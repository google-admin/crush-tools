if COND_PERL_DBI_PRESENT
WITH_DBI = $(NEEDS_PERL_DBI)
endif

if COND_PERL_DATE_CALC_PRESENT
WITH_DATE_CALC = $(NEEDS_PERL_DATE_CALC)
endif

SUBDIRS = bin libcrush perllib aggregate aggregate2 buffer convdate \
          cutfield deltaforce foreach_parallel funiq grepfield hashjoin \
          indexof mergekeys pivot range reorder subtotal tochar truncfield \
          add_field fieldsplit translate_field findfiles csvformat \
          calcfield find_not_processed filterkeys treesort \
          $(WITH_DATE_CALC) $(WITH_DBI) 
