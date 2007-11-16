# Copyright (C) 2007 jeremy hinds <jhinds@doubleclick.com>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

include aminclude.am

if DX_COND_man
MANPAGES = $(wildcard @DX_DOCDIR@/man/man3/*.h.3)
dist_man1_MANS = $(MANPAGES)

$(dist_man1_MANS): doxygen-doc
endif

MOSTLYCLEANFILES = $(DX_CLEANFILES)
EXTRA_DIST = reconf configure $(DX_CONFIG)
SUBDIRS = m4 src doc
