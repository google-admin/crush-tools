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


/*
struct aggregation {
	u_int32_t counts[8];
	double sums[8];
};
*/

struct aggregation {
	u_int32_t *counts;
	double *sums;
	u_int32_t *average_counts;
	double *average_sums;
};

void extract_fields_to_string (char *line, char *destbuf, size_t destbuf_sz, int *fields, size_t nfields, char *delim ) ;
void decrement_values ( int *array, size_t sz ) ;
int print_keys_sums_counts_avgs ( char *key, struct aggregation *val ) ;
int ht_print_keys_sums_counts_avgs ( void * htelem ) ;
int key_strcmp ( char **a, char **b ) ;
int float_str_precision ( char *d ) ;


/** @brief allocates and initializes an aggregation struct
  * 
  * @param nsum number of fields to sum
  * @param ncount number of fields to count
  * 
  * @return a shiny new, zeroed-out structure
  */
struct aggregation * alloc_agg ( int nsum, int ncount, int naverage ) ;

void free_agg( struct aggregation *agg );


#endif /* AGGREGATE_H */
