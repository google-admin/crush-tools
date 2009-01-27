m4_dnl we need to get rid of '#' as the comment character so that m4 will
m4_dnl expand macros in shebang lines
m4_changecom()m4_dnl
m4_dnl change the quote characters so that backticks don't look like the
m4_dnl beginning of strings
m4_changequote(<++,++>)m4_dnl
