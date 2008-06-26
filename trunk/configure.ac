AC_INIT([crush],
        [0.1],
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

AC_PROG_CC
AC_PATH_PROG(PERL, perl, none)

AC_PROG_INSTALL
if test "$INSTALL" = "build/install-sh -c"; then
  INSTALL="$abs_srcdir/$INSTALL"
fi

AC_SUBST([MKDIR], ["$INSTALL -d"])

AC_CHECK_PROG(CGENER, [cgener], [cgener], [not found], [$PATH])

if test "$CGENER" = "not found"; then
	AC_MSG_ERROR(
    [The cgener application is required to build this application.
     It is available from http://code.google.com/p/cgener/], 1)
fi

AC_PROG_LIBTOOL

# cygwin has fcntl.h under sys/
AC_CHECK_HEADERS([fnctl.h sys/fcntl.h unistd.h err.h locale.h sys/types.h \
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
src/Makefile
src/libcrush/Makefile
src/aggregate/Makefile
src/aggregate2/Makefile
src/buffer/Makefile
src/convdate/Makefile
src/cutfield/Makefile
src/deltaforce/Makefile
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
src/truncfield/Makefile
])

dnl src/add_field/Makefile
dnl src/dbstream/Makefile
dnl src/calcfields/Makefile
dnl src/csvformat/Makefile
dnl src/dates_in_range/Makefile
dnl src/deltadays/Makefile
dnl src/fieldsplit/Makefile
dnl src/find_not_processed/Makefile
dnl src/findfiles/Makefile
dnl src/translate_field/Makefile

AC_OUTPUT
