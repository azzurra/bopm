/*
Copyright (C) 2002  Erik Fears
 
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
 
      Foundation, Inc.
      59 Temple Place - Suite 330
      Boston, MA  02111-1307, USA.
 
*/

#include "setup.h"

#ifdef STDC_HEADERS
# include <string.h>
# include <stdlib.h>
#endif

#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>

#include "options.h"
#include "irc.h"
#include "log.h"
#include "misc.h"
#include "opercmd.h"
#include "scan.h"
#include "config.h"
#include "extern.h"
#include "malloc.h"
#include "list.h"
#include "stats.h"

/* List of active commands */

list_t *COMMANDS = NULL;


static struct Command *command_create(unsigned short type, char *param, char *irc_nick, struct ChannelConf *target);
static void command_free(struct Command *);

static void cmd_check(char *, char *, struct ChannelConf *);
static void cmd_stat(char *, char *, struct ChannelConf *);
static void cmd_fdstat(char *, char *, struct ChannelConf *);
static void cmd_quit(char *, char *, struct ChannelConf *);
static void cmd_restart(char *, char *, struct ChannelConf *);
static void cmd_exception(char *, char *, struct ChannelConf *);

#if 0
static void cmd_op(char *, char *, struct ChannelConf *);
#endif

static struct OperCommandHash COMMAND_TABLE[] =
   {
      {"CHECK",  	cmd_check     	},
      {"SCAN",   	cmd_check     	},
      {"STAT",   	cmd_stat      	},
      {"STATS",  	cmd_stat      	},
      {"STATUS", 	cmd_stat      	},
      {"FDSTAT", 	cmd_fdstat    	},
      {"QUIT",   	cmd_quit      	},
      {"RESTART",	cmd_restart   	},
      {"EXCEPTION",	cmd_exception	}
/*    {"OP",    	cmd_op     	} */
   };



/* command_init
 * 
 *    Do command initialization
 *
 * Parameters: NONE
 * Return: NONE
 *
 */

void command_init()
{
   if(COMMANDS == NULL)
      COMMANDS = list_create();
}




/* command_timer
 *
 *    Perform ~1 second actions.
 *
 * Parameters: NONE
 * 
 * Return: NONE
 *
 */

void command_timer()
{

   static unsigned short interval;

   node_t *node, *next;
   struct Command *cs;
   time_t present;

   /* Only perform command removal every COMMANDINTERVAL seconds */
   if(interval++ < COMMANDINTERVAL)
      return;
   else
      interval = 0;

   time(&present);

   LIST_FOREACH_SAFE(node, next, COMMANDS->head)
   {
      cs = (struct Command *) node->data;
      if((present - cs->added) > COMMANDTIMEOUT)
      {
         command_free(cs);
         list_remove(COMMANDS, node);
         node_free(node);
      }
      else   /* Since the queue is in order, it's also ordered by time, no nodes after this will be timed out */
         return;
   }
}


/* command_parse
 *
 *    Parse a command to bopm (sent to a channel bopm is on). The command is parsed
 *    from the parameters, and if it is a known command it is stored in a queue. A
 *    userhost is performed on the user to check if they are an IRC operator. When
 *    a reply is returned (command_userhost), the command will be executed.
 *
 * Parameters:
 *    command: Command sent (including parameters)
 *    msg: Original PRIVMSG containing the command
 *    target: Channel command was sent to (we only got this far if there was only one recipient)
 *    source_p: Operator (hopefully) that sent the command.
 *
 */

void command_parse(char *command, char *msg, struct ChannelConf *target,
      struct UserInfo *source_p)
{
   unsigned int i;
   char *param; /* Parsed parameters */

   struct Command *cs;
   node_t *node;

   USE_VAR(msg);

   if(OPT_DEBUG)
   {
      log_printf("COMMAND -> Parsing command (%s) from %s [%s]", command,
            source_p->irc_nick, target->name);
   }

   /* Only allow COMMANDMAX commands in the queue */
   if(LIST_SIZE(COMMANDS) >= COMMANDMAX)
      return;

   /* Parameter is the first character in command after the first space.
      param will be NULL if:
      1. There was no space
      2. There was a space but it was the last character in command, in which case
         param = '\0'
   */

   /* Skip past the botname/!all */
   command = strchr(command, ' ');

   /* There is no command OR
      there is at least nothing
      past that first space.  */
   if(command == NULL ||
         command++ == NULL)
      return;


   /* Find the parameters */
   param = strchr(command, ' ');

   if(param != NULL)
   {
      *param = '\0';
      param++;
   }
   else
      param = "";

   log_printf("COMMAND -> parsed [%s] [%s]", command, param);

   /* Lookup the command in the table */
   for(i = 0; i < sizeof(COMMAND_TABLE) / sizeof(struct OperCommandHash); i++)
   {
      if(strcasecmp(command, COMMAND_TABLE[i].command) == 0)
      {
         /* Queue this command */
         cs = command_create(i, param, source_p->irc_nick, target);
         node = node_create(cs);
         list_add(COMMANDS, node);
      }
   }

   irc_send("USERHOST %s", source_p->irc_nick);
}




/* command_create
 * 
 *    Create a Command struct.
 *  
 * Parameters:
 *    type: Index in COMMAND_TABLE
 *    param: Parameters to the command (NULL if there are not any)
 *    irc_nick: Nickname of user that initiated the command
 *    target: Target channel (target is ALWAYS a channel)
 *
 * Return:
 *    Pointer to new Command
 */

static struct Command *command_create(unsigned short type, char *param, char *irc_nick, struct ChannelConf *target)
{
   struct Command *ret;

   ret = MyMalloc(sizeof *ret);

   ret->type = type;

   if(param != NULL)
      ret->param = DupString(param);
   else
      ret->param = NULL;

   ret->irc_nick = (char *) DupString(irc_nick);
   ret->target = target; /* FIXME: This needs fixed if rehash is implemented */

   time(&(ret->added));

   return ret;

}



/* command_free
 * 
 *   Free a command struct
 *
 * Parameters:
 *   command: Command struct to free
 *   
 * Return: NONE
 */

static void command_free(struct Command *command)
{
   if(command->param != NULL)
      MyFree(command->param);
   MyFree(command->irc_nick);
   MyFree(command);
}




/* command_userhost
 *
 *    A 302 reply was received. The reply is parsed to check if the
 *    user was an operator. If so any commands they had queued are 
 *    executed.
 *  
 * Parameters:
 *    reply: Reply to USERHOST    (ex: :grifferz*=+goats@pc-62-30-219-54-pb.blueyonder.co.uk)
 *
 * Return: NONE
 * 
 */

void command_userhost(char *reply)
{
   node_t *node, *next;
   struct Command *cs;
   char *tmp;

   int oper = 0;

   tmp = strchr(reply, '=');

   /* They quit, ignore it */
   if (!tmp)
      return;

   /* Operators have a * flag in a USERHOST reply */
   if (*(tmp - 1) == '*')
      oper = 1;

   /* Null terminate it so tmp = the oper's nick */
  if(oper) 
     *(--tmp) = '\0';
  else
     *(tmp) = '\0';

   /* Find any queued commands that match this user */
   LIST_FOREACH_SAFE(node, next, COMMANDS->head)
   {
      cs = (struct Command *) node->data;

      if(strcmp(cs->irc_nick, reply) == 0)
      {
         if(oper)
            COMMAND_TABLE[cs->type].handler(cs->param, cs->irc_nick, cs->target);

         /* Cleanup the command */
         command_free(cs);
         list_remove(COMMANDS, node);
         node_free(node);
      }
   }
}
/*
 * cmd_exception
 *
 * Lists, adds, or removes and exception from the list
 *
 * Paramters: 
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 *
 */

static void cmd_exception(char *param, char *source, struct ChannelConf *target)
{
	char 	**params;
	int	i = 0;
	node_t	*node;
	if (param == NULL)
		return; //we don't have enough params
	params = (char **) malloc(sizeof(char *) * 2); //we need max 2 params 
	params = (char **) memset(params, 0, sizeof(char *) * 2);
	params[0] = strtok(param, " ");
	if (params[0] == NULL)
		return;
	params[1] = strtok(NULL, " "); //WARNING: this could be NULL
	if (!strcasecmp(params[0], "LIST"))
	{
		irc_send("PRIVMSG %s :\002***\002Exception List: \002***\002", target->name);
		LIST_FOREACH(node, ExemptItem->masks->head)
		{
			irc_send("PRIVMSG %s :%d) %s", target->name, ++i, node->data);
		}
		irc_send("PRIVMSG %s :\002***\002End Of List \002***\002", target->name);
	}
	else if (!strcasecmp(params[0], "ADD"))
	{
		//FIXME: Adding in memory is simple...but we need to save this in the conf... :(
		if (params[1] == NULL)
			return;
		node = node_create(strdup(params[1]));
		list_add(ExemptItem->masks, node);
		irc_send("PRIVMSG %s :Ok %s, done.", target->name, source);
		
	} else if (!strcasecmp(params[0], "DEL")) {
		//FIXME: ...The same story...
		if (params[1] == NULL)
			return;
		LIST_FOREACH(node, ExemptItem->masks->head)
		{
			if (!strcasecmp(params[1], node->data))
			{
				//found!
				list_remove(ExemptItem->masks, node);
				irc_send("PRIVMSG %s :Ok %s, done",target->name, source);
				return;
			}
		}
		irc_send("PRIVMSG %s :Sorry %s, entry not found.", target->name, source);
	}
	free(params);
}
/*
 * cmd_restart
 *
 * Restarts the Proxy Monitor (this could be done via /kill but that
 * was very ugly :S
 *
 * Paramters: 
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 *
 */
static void cmd_restart(char *param, char *source, struct ChannelConf *target)
{
	log_printf("Got RESTART command from %s", source);
	irc_send("QUIT :Restarting...");
	main_restart();
}

/* cmd_quit
 *
 * Shuts down the Proxy Monitor
 *
 * Paramters: 
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 *
 */


static void cmd_quit(char *param, char *source, struct ChannelConf *target)
{
	log_printf("Shutdown requested by %s", source);
	irc_send("QUIT :Shutted Down!");
	exit(0); /*R.I.P.*/
}

/* cmd_check
 *
 *    Start a manual scan on given IP. Parameter MUST be an IP. BOPM should not
 *    have to waste any time resolving a hostname.
 *
 * Parameters:
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 *
 */

static void cmd_check(char *param, char *source, struct ChannelConf *target)
{
   USE_VAR(source);

   scan_manual(param, target);
}



/* cmd_stat
 *
 *   Send output of stats to channel.
 *
 * Parameters:
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 */

static void cmd_stat(char *param, char *source, struct ChannelConf *target)
{
   USE_VAR(param);
   USE_VAR(source);

   stats_output(target->name);
}


/* cmd_fdstat
 *
 *   Send output of stats to channel.
 *
 * Parameters:
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 */

static void cmd_fdstat(char *param, char *source, struct ChannelConf *target)
{
   USE_VAR(param);
   USE_VAR(source);

   fdstats_output(target->name);
}


/* cmd_op
 *
 *   Op a user on the channel.
 *
 * Parameters:
 *    param: Parameters of the command
 *    source: irc_nick of user who requested the command
 *    target: channel command was sent to
 *
 * XXX - Doesn't seem to currently be in use anywhere?
 *  -grifferz
 */

#if 0
static void cmd_op(char *param, char *source, struct ChannelConf *target)
{
   irc_send("MODE %s +o %s", target->name, param);

   USE_VAR(source);
}
#endif
