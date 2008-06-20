# Copyright (C) 2007 jeremy hinds <jhinds@doubleclick.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AC_INIT([jdatastructs],
        [1.1.0],
        [jeremy hinds jhinds@doubleclick.net],
        [jdatastructs])
AC_CONFIG_AUX_DIR(config)
define([DOXYGEN_FILE], [doc/jdatastructs.doxygen])

AC_CONFIG_SRCDIR([src/linklist.c])
AC_DEFINE([YOUR_OS], [], [Description])
AM_CONFIG_HEADER(config.h)

DX_HTML_FEATURE(ON)
DX_CHM_FEATURE(OFF)
DX_CHI_FEATURE(OFF)
DX_MAN_FEATURE(ON)
DX_RTF_FEATURE(OFF)
DX_XML_FEATURE(OFF)
DX_PDF_FEATURE(ON)
DX_PS_FEATURE(OFF)
DX_INIT_DOXYGEN(jdatastructs, doc/jdatastructs.doxygen)

AM_INIT_AUTOMAKE([dist-bzip2])

LF_CONFIGURE_CC
LF_HOST_TYPE
LF_SET_WARNINGS
AC_PROG_LIBTOOL

AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h])
AC_C_CONST
AC_TYPE_SIZE_T
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([
   Makefile
   README
   doc/Makefile
   m4/Makefile
   src/Makefile
])

AC_OUTPUT
