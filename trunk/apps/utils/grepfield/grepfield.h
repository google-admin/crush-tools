#include <regex.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ffutils.h>

#ifndef GREPFIELD_H
#define GREPFIELD_H

#ifndef REG_OK
#define REG_OK 0
#endif


char * scan_wholeline(char **, ssize_t *, char *, char *, int );
char * scan_field(char **, ssize_t *, char *, char *, int );
void re_perror(int err_code, regex_t pattern);

#endif
