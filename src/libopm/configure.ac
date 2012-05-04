dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT(libopm, [0.1], [devel@ml.azzurra.org])
AC_CANONICAL_TARGET
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/libopm.h)
AM_CONFIG_HEADER(src/setup.h)

AM_INIT_AUTOMAKE([1.11 silent-rules])
AC_PREFIX_DEFAULT([/usr/local/libopm])
AM_MAINTAINER_MODE

# for documentation purposes
DOX_DIR_HTML=api
DOX_DIR_LATEX=latex
DOX_DIR_MAN=man

AC_SUBST(DOX_DIR_HTML)
AC_SUBST(DOX_DIR_LATEX)
AC_SUBST(DOX_DIR_MAN)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

dnl check if we need -lsocket or -lnsl
ETR_SOCKET_NSL

dnl Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADERS(sys/poll.h, have_poll_sys_h=yes, have_sys_poll_h=no)

AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h strings.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_HEADER_TIME

AC_CHECK_FUNCS(snprintf vsnprintf)

dnl if they want select() or they don't have poll() then we need to check
dnl that we actually have select()
if test "$have_sys_poll_h" = "no"; then
 AC_CHECK_FUNCS(select, have_select=yes, have_select=no)
 if test "$have_select" = "no"; then
   AC_MSG_ERROR([No select() implementation found])
 fi
fi

AC_CHECK_FUNCS(inet_aton inet_pton)

dnl Check if we can use gethostbyname2 for ipv6
AC_CHECK_FUNCS(gethostbyname gethostbyname2)

dnl AIX fun
AC_C_BIGENDIAN

dnl Solaris has to be weird doesn't it...
AC_CHECK_LIB(socket, socket, AC_SUBST(LSOCKET, [-lsocket]))
AC_CHECK_LIB(nsl, gethostbyname, AC_SUBST(LNSL, [-lnsl]))

LTLIBOBJS=`echo "$LIB@&t@OBJS" |
           sed 's,\.[[^.]]* ,.lo ,g;s,\.[[^.]]*$,.lo,'`
AC_SUBST(LTLIBOBJS)

dnl This comes last because configure won't actually work with -Werror in
dnl the CFLAGS
AC_ARG_WITH(werror,
    AC_HELP_STRING([--with-werror],
    [use -Werror to abort compilation on any warning]),
    [CFLAGS="$CFLAGS -Werror"])

AC_OUTPUT(Makefile src/Makefile doc/Makefile doc/Doxyfile)
