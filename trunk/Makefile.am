
AUX_DIST = $(ac_aux_dir)/config.guess \
					 $(ac_aux_dir)/config.sub \
					 $(ac_aux_dir)/install-sh \
					 $(ac_aux_dir)/ltconfig \
					 $(ac_aux_dir)/ltmain.sh \
					 $(ac_aux_dir)/mdate-sh \
					 $(ac_aux_dir)/missing \
					 $(ac_aux_dir)/mkinstalldirs

AUX_DIST_EXTRA = $(ac_aux_dir)/readline.m4 \
								 $(ac_aux_dir)/sys_errlist.m4 \
								 $(ac_aux_dir)/sys_siglist.m4

EXTRA_DIST = COPYING cgener

# make sure the AUX files get included in the distribution package
dist-hook:
	(cd $(distdir) && mkdir $(ac_aux_dir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
		cp $$file $(distdir)/$$file; \
	done

SUBDIRS = src
INCLUDES = -I$(top_srcdir)/include
PERL = @PERL@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
