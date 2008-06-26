/*****************************************
   Copyright 2008 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_REGEX_H
# include <regex.h>
#endif

#if HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#if HAVE_STDLIB_H
# include <stdlib.h>
#endif

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
