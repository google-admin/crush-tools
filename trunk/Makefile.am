
AUX_DIST = $(ac_aux_dir)/config.guess \
					 $(ac_aux_dir)/config.sub \
					 $(ac_aux_dir)/depcomp \
					 $(ac_aux_dir)/install-sh \
					 $(ac_aux_dir)/ltmain.sh \
					 $(ac_aux_dir)/missing

EXTRA_DIST = COPYING cgener

# make sure the AUX files get included in the distribution package
dist-hook:
	(cd $(distdir) && if ! test -d $(ac_aux_dir); then mkdir $(ac_aux_dir); fi)
	for file in $(AUX_DIST); do \
		cp $$file $(distdir)/$$file; \
	done

SUBDIRS = src
INCLUDES = -I$(top_srcdir)/include
PERL = @PERL@

libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck
