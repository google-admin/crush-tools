AC_INIT([crush],
        [2008-10_trunk],
        [http://code.google.com/p/crush-tools/])
AM_CONFIG_HEADER(src/include/config.h)
AC_CONFIG_SRCDIR(src/include/ffutils.h)
AC_CONFIG_AUX_DIR([build])
AM_INIT_AUTOMAKE

dnl -D_GNU_SOURCE allows use of getline()
AC_GNU_SOURCE

LT_INIT
AC_SUBST([LIBTOOL_DEPS])

AC_SUBST(ac_aux_dir)
AC_SUBST([abs_srcdir], ["`cd $srcdir && pwd`"])
AC_SUBST([abs_builddir], ["`pwd`"])
AC_SUBST([CGENER], [$abs_srcdir/cgener])
AC_SUBST([BUILD_DATE_TIME], ["`date +%Y-%m-%d-%H:%M:%S`"])

AC_PROG_CC
AC_PATH_PROG(M4, m4, none)
if test "$M4" = "none"; then
  AC_MSG_ERROR("m4 was not found in your PATH.  Please install m4 or update your PATH variable and rerun $0")
fi

AC_PATH_PROG(PERL, perl, none)
if test "$PERL" = "none"; then
  AC_MSG_ERROR("perl was not found in your PATH.  Please install perl or update your PATH variable and rerun $0")
fi
CRUSH_REQUIRE_PERL_MODULE(Getopt::Long)
CRUSH_CHECK_PERL_MODULE(DBI, PERL_DBI_PRESENT)
CRUSH_CHECK_PERL_MODULE(Date::Calc, PERL_DATE_CALC_PRESENT)
AC_SUBST([NEEDS_PERL_DBI], ["dbstream"])
AC_SUBST([NEEDS_PERL_DATE_CALC], ["dates_in_range deltadays"])

AC_PROG_INSTALL
if test "$INSTALL" = "build/install-sh -c"; then
  INSTALL="$abs_srcdir/$INSTALL"
fi

AC_SUBST([MKDIR], ["$INSTALL -d"])

AC_PROG_LIBTOOL

# cygwin has fcntl.h under sys/
AC_CHECK_HEADERS([fcntl.h sys/fcntl.h unistd.h err.h locale.h sys/types.h \
                  sys/stat.h regex.h assert.h])
AC_HEADER_STDC
AC_C_CONST
AC_TYPE_SIZE_T

AC_DEFINE(_LARGEFILE64_SOURCE, [1],
          [make O_LARGEFILE open flag visible if available])

AC_CHECK_FUNCS([open64 getline fgetln])

AC_ARG_ENABLE(maintainer-mode,
AS_HELP_STRING([--enable-maintainer-mode],
               [enable debugging and extra compiler warnings]),
               [if test "$enableval" = "yes"; then
                  if test "$GCC" = "yes"; then
                    CFLAGS=["`echo $CFLAGS' ' | sed -e 's/-O[^ ]* //g'`"]
                    CXXFLAGS=["`echo $CXXFLAGS' ' | sed -e 's/-O[^ ]* //g'`"]
                    CFLAGS="$CFLAGS -Wall -g -DCRUSH_DEBUG"
                    CXXFLAGS="$CXXFLAGS -Wall -g -DCRUSH_DEBUG"
                  fi
                fi
               ]
             )

dnl TODO: add enable-profiling - requires a static build

AC_CONFIG_FILES([Makefile
contrib/Makefile
src/Makefile
src/bin/Makefile
src/include/Makefile
src/include/crush_version.h
src/libcrush/Makefile
src/perllib/Makefile
src/perllib/crush_version.pl
src/add_field/Makefile
src/aggregate/Makefile
src/aggregate2/Makefile
src/buffer/Makefile
src/calcfield/Makefile
src/convdate/Makefile
src/csvformat/Makefile
src/cutfield/Makefile
src/dates_in_range/Makefile
src/dbstream/Makefile
src/deltadays/Makefile
src/deltaforce/Makefile
src/fieldsplit/Makefile
src/find_not_processed/Makefile
src/findfiles/Makefile
src/foreach_parallel/Makefile
src/funiq/Makefile
src/grepfield/Makefile
src/indexof/Makefile
src/mergekeys/Makefile
src/pivot/Makefile
src/range/Makefile
src/reorder/Makefile
src/subtotal/Makefile
src/tochar/Makefile
src/translate_field/Makefile
src/truncfield/Makefile
])

AC_OUTPUT

if test $PERL_DBI_PRESENT = no; then
  AC_MSG_WARN([The DBI Perl module was not found.])
  AC_MSG_WARN([$NEEDS_PERL_DBI will not be built])
  AC_MSG_WARN([You may want to run "cpan DBI" and rerun $0])
fi

if test $PERL_DATE_CALC_PRESENT = no; then
  AC_MSG_WARN([The Date::Calc Perl module was not found.])
  AC_MSG_WARN([$NEEDS_PERL_DATE_CALC will not be built.])
  AC_MSG_WARN([You may want to run "cpan Date::Calc" and rerun $0])
fi
