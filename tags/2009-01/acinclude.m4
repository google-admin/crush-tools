
# check to see if a Perl module is installed, and fail if it is not
AC_DEFUN([CRUSH_REQUIRE_PERL_MODULE],[dnl
  AC_MSG_CHECKING([for perl module $1])
  $PERL -M$1 -e exit >/dev/null 2>&1
  if test $? -ne 0; then
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([you should fire up "$PERL -MCPAN -e 'install $1'", and then re-run configure])
  else
    AC_MSG_RESULT([yes])
  fi
])

# check to see if a Perl module is installed and set a variable and
# conditional if it is
AC_DEFUN([CRUSH_CHECK_PERL_MODULE],[dnl
  AC_MSG_CHECKING([for perl module $1])
  $PERL -M$1 -e exit >/dev/null 2>&1
  if test $? -ne 0; then
    AC_MSG_RESULT([no])
    AC_SUBST($2, "no")
  else
    AC_MSG_RESULT([yes])
    AC_SUBST($2, "yes")
  fi
  AM_CONDITIONAL([COND_$2], [test x$$2 = xyes])
])

