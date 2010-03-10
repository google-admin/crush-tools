/********************************
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
 ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <crush/ffutils.h>

void usage(char *bin);

int main(int argc, char *argv[]) {
  int i, n, *array = NULL;
  size_t array_sz = 0;

  if (argc < 2 || strncmp(argv[1], "-h", 2) == 0) {
    usage(argv[0]);
    return (1);
  }

  n = expand_nums(argv[1], &array, &array_sz);
  for (i = 0; i < n; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
  free(array);

  return (0);
}

void usage(char *bin) {
  fprintf(stderr,
          "\nexpands a list of numbers & number ranges into individual values.\n\n");
  fprintf(stderr, "usage: %s <numbers>\n\n", bin);
  fprintf(stderr,
          "<numbers> may be a mixture of comma-separated numbers and ranges (no spaces).\n");
  fprintf(stderr, "example: %s 1,2,6-10\n\n", bin);
}
