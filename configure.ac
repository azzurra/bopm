dnl Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([bopm], [3.1.3], [devel@ml.azzurra.org])
AC_CANONICAL_TARGET
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(src/opercmd.h)
AC_CONFIG_HEADERS(src/setup.h)

AM_INIT_AUTOMAKE([1.11 no-dist-gzip dist-bzip2 silent-rules])
AC_PREFIX_DEFAULT([\${HOME}/bopm])
AM_MAINTAINER_MODE

dnl Checks for programs.
AC_PROG_CC
AC_ISC_POSIX
AC_PROG_INSTALL
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_LIBTOOL

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADERS(sys/poll.h, have_poll_sys_h=yes, have_sys_poll_h=no)
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h strings.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_SIGNAL

AC_CHECK_HEADERS(stdint.h sys/types.h sys/limits.h inttypes.h)

AC_CHECK_SIZEOF(int8_t)
AC_CHECK_SIZEOF(int16_t)
AC_CHECK_SIZEOF(int32_t)
AC_CHECK_SIZEOF(int64_t)
AC_CHECK_SIZEOF(uint8_t)
AC_CHECK_SIZEOF(uint16_t)
AC_CHECK_SIZEOF(uint32_t)
AC_CHECK_SIZEOF(uint64_t)
AC_CHECK_SIZEOF(intptr_t)
AC_CHECK_SIZEOF(uintptr_t)

dnl Just in case we don't have exact width integers
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)
AC_CHECK_SIZEOF(__int64)
AC_CHECK_SIZEOF(void *)

dnl Check size of generic types
AC_CHECK_SIZEOF(time_t)
AC_CHECK_SIZEOF(size_t)

dnl Check u_types
AC_CHECK_TYPES([uint, u_char, u_short, u_long, u_int, ssize_t])

dnl Check time_t signedness
AC_CACHE_CHECK([whether time_t is signed], bopm_cv_time_t_signed, [
AC_RUN_IFELSE([AC_LANG_SOURCE([
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#ifdef HAVE_TIME_H
#include <time.h>
#endif
int main(int c, char**v) { if (((time_t)-1)<0) return 1; else return 0; }])],
  bopm_cv_time_t_signed=no, bopm_cv_time_t_signed=yes, bopm_cv_time_t_signed=cross)
])

if test "$bopm_cv_time_t_signed" = cross; then
  AC_MSG_NOTICE([Cross compiling: assuming that time_t is signed.])
fi

if test "$bopm_cv_time_t_signed" != no; then
  AC_DEFINE([TIME_T_IS_SIGNED], 1,
            [Define to 1 iff time_t is signed])
fi

dnl check if we need -lsocket or -lnsl
ETR_SOCKET_NSL

AC_PROG_GCC_TRADITIONAL
AC_CHECK_FUNCS(inet_aton inet_pton strdup strstr strftime snprintf vsnprintf)

dnl Check if we can use gethostbyname2 for ipv6
AC_CHECK_FUNCS(gethostbyname gethostbyname2)

dnl Check if we're living in a 2s-complement world
AC_CACHE_CHECK([whether we are living in a 2s-complement world], bopm_cv_twos_complement,
[AC_RUN_IFELSE([AC_LANG_SOURCE(
[[int main () { int problem = ((-99) != (~99)+1);
return problem ? 1 : 0; }]])],
       [bopm_cv_twos_complement=yes],
       [bopm_cv_twos_complement=no],
       [bopm_cv_twos_complement=cross])])

if test "$bopm_cv_twos_complement" = cross ; then
  # Cross-compiling ?!?!?!?!
  AC_MSG_NOTICE([Cross-compiling: we will assume that negative integers are represented with 2s-complement.])
fi

if test "$bopm_cv_twos_complement" != no ; then
  AC_DEFINE([USING_TWOS_COMPLEMENT], 1,
            [Define to 1 iff we represent negative integers with 2s-complement])
fi

dnl Endianess
AC_C_BIGENDIAN

AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(long)

AC_CONFIG_SUBDIRS(src/libopm)

dnl This comes last because configure won't actually work with -Werror in
dnl the CFLAGS
AC_ARG_WITH(werror,
    AC_HELP_STRING([--with-werror],
    [use -Werror to abort compilation on any warning]),
    [CFLAGS="$CFLAGS -Werror"])

AC_OUTPUT(Makefile src/Makefile)
