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
#include <string.h>
#include <sys/types.h>
#include <assert.h>
#include <locale.h>

#include <ffutils.h>
#include <hashtbl.h>
#include <linklist.h>

#ifndef AGGREGATE_H
#define AGGREGATE_H

struct agg_conf_field {
  int *indexes;  /**< array of field indexes. */
  size_t size;  /**< size of the index array. */
  size_t count;  /**< number of elems in the index array. */
  int *precisions;  /**< precision of output for each field. */
};

struct agg_conf {
  struct agg_conf_field keys;
  struct agg_conf_field counts;
  struct agg_conf_field sums;
  struct agg_conf_field averages;
  struct agg_conf_field mins;
  struct agg_conf_field maxs;
};

struct aggregation {
  u_int32_t *counts;
  double *sums;
  u_int32_t *average_counts;
  double *average_sums;
  double *numeric_mins;
  /* for each min field, whether a populated input field has been found yet. */
  char *mins_initialized;
  double *numeric_maxs;
  /* for each max field, whether a populated input field has been found yet. */
  char *maxs_initialized;
  /* char *string_mins; */
  /* char *string_maxs; */
};

int configure_aggregation(struct agg_conf *conf, struct cmdargs *args,
                          const char *header, const char *delim);
void extract_fields_to_string(char *line, char *destbuf, size_t destbuf_sz,
                              int *fields, size_t nfields, char *delim,
                              char *suffix);
void decrement_values(int *array, size_t sz);
int print_keys_and_agg_vals(char *key, struct aggregation *val);
int ht_print_keys_and_agg_vals(void *htelem);
int key_strcmp(char **a, char **b);
int float_str_precision(char *d);


/** @brief allocates and initializes an aggregation struct
  * 
  * @param nsum number of fields to sum
  * @param ncount number of fields to count
  * 
  * @return a shiny new, zeroed-out structure
  */
struct aggregation *alloc_agg(int nsum, int ncount, int naverage, int nmin,
                              int nmax);

void free_agg(struct aggregation *agg);


#endif /* AGGREGATE_H */
