
AC_INIT([mempool],
        [1.0],
        [jeremy hinds jhinds@doubleclick.com],
        [mempool])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/mempool.c])
AC_DEFINE([YOUR_OS], [], [Description])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

LF_CONFIGURE_CC
LF_HOST_TYPE
LF_SET_WARNINGS
AC_PROG_RANLIB
AC_PROG_LIBTOOL

AC_CHECK_HEADERS([stdlib.h string.h])
AC_C_CONST
AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memcpy])

AC_CONFIG_FILES([
   Makefile
   README
   doc/Makefile
   src/Makefile
])

AC_OUTPUT
