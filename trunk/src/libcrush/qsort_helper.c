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

#include <qsort_helper.h>

int qsort_strcmp ( const char **a, const char **b ) {
	return strcmp(*a, *b);
}

int qsort_intcmp ( const int *a, const int *b ) {
	return *a - *b;
}

int qsort_uintcmp ( const unsigned int *a, const unsigned int *b ) {
	return *a - *b;
}

