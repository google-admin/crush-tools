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

#include <stdio.h>

#include "ffutils.h"

int main(int argc, char *argv[]) {

  char *date_a, *date_b;
  char line[64];

  int cmp;

  while (fgets(line, 64, stdin)) {

    line[19] = '\0';
    line[39] = '\0';

    date_a = line;
    date_b = &(line[20]);

    cmp = mdyhms_datecmp(date_a, date_b);
    /*
       printf("%s %s %s\n",
       date_a,
       cmp == 0 ? "=" : (cmp > 0 ? ">" : "<"),
       date_b );
     */
  }

  return 0;
}
