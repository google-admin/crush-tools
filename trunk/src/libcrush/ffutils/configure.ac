# Copyright (C) 2007 jeremy hinds <jhinds@doubleclick.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([ffutils],
        [1],
        [jeremy hinds jhinds@doubleclick.com],
        [ffutils])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/ffutils.c])
AC_DEFINE([YOUR_OS], [], [Description])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([dist-bzip2])

dnl AC_GNU_SOURCE allows use of getline()
AC_DEFINE(_LARGEFILE64_SOURCE, [1], [make O_LARGEFILE open flag visible if available])

LF_CONFIGURE_CC
LF_HOST_TYPE
LF_SET_WARNINGS
AC_PROG_RANLIB
AC_PROG_LIBTOOL


AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])
AC_C_CONST
AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strchr strstr open64 getline fgetln])

AC_CONFIG_FILES([
   Makefile
   README
   doc/Makefile
   m4/Makefile
   src/Makefile
])

AC_OUTPUT
