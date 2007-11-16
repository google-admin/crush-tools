# Copyright (C) 2007 jeremy hinds <jhinds@doubleclick.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

lib_LTLIBRARIES = libjdatastructs.la

libjdatastructs_la_SOURCES = bstree.c GeneralHashFunctions.c hashfuncs.c hashtbl2.c hashtbl.c linklist.c queue.c

include_HEADERS = bstree.h hashfuncs.h hashtbl2.h hashtbl.h ht2_GeneralHashFunctions.h linklist.h queue.h 

libjdatastructs_la_CFLAGS = -O2 -Wall

libjdatastructs_la_LDFLAGS = -version-info 1:0:1
