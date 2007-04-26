/********************************
  copyright
 ********************************/
#include "aggregate_main.h"
#include "aggregate.h"


int ncounts;		/* the number of count fields */
int nsums;		/* the number of sum fields */
int *sum_precisions;	/* array of precisions for the sum fields */
char *delim;


/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int aggregate ( struct cmdargs *args, int argc, char *argv[], int optind ){

	int i, n;

	hashtbl_t aggregations;
	llist_t *hash_keys;

	int nkeys;		/* the number of key fields */
	int *key_fields;	/* array of key field indexes */
	int *count_fields;	/* array of count field indexes */
	int *sum_fields;	/* array of sum field indexes */

	size_t n_hash_elems;

	FILE *in;		/* input file */
	char *inbuf;		/* buffer for a line of input */
	size_t inbuf_sz;	/* size of the input buffer */

	char *outbuf;		/* buffer for a line of output */
	size_t outbuf_sz;	/* size of the output buffer */

	char default_delim[] = { 0xFE, 0x00 };	/* default delimiter string */

	delim = args->delim;
	if ( ! delim )
		delim = getenv("DELIMITER");

	if ( delim )
		expand_chars(delim);
	else
		delim = default_delim;

	nkeys = ncounts = nsums = 0;
	key_fields = count_fields = sum_fields = NULL;

	nkeys = splitnums( args->keys, &key_fields, 0 );

	assert(nkeys != 0);

	decrement_values( key_fields, nkeys );

	if ( args->sums ) {
		nsums = splitnums( args->sums, &sum_fields, 0 );
		decrement_values( sum_fields, nsums );
		sum_precisions = malloc(sizeof(int) * nsums);
		memset(sum_precisions, 0, sizeof(int) * nsums); 
	}

	if ( args->counts ) {
		ncounts = splitnums( args->counts, &count_fields, 0 );
		decrement_values( count_fields, ncounts );
	}

#ifdef DEBUG
	printf("%d keys: ", nkeys);
	for ( i = 0; i < nkeys; i++ )
		printf("%d ", key_fields[i]);
	printf("\n%d sums: ", nsums);
	for ( i = 0; i < nsums; i++ )
		printf("%d ", sum_fields[i]);
	printf("\n%d counts: ", ncounts);
	for ( i = 0; i < ncounts; i++ )
		printf("%d ", count_fields[i]);
	printf("\n\n");
#endif

	if ( optind == argc )
		in = stdin;
	else
		in = nextfile( argc, argv, &optind, "r" );

	if ( in == NULL )
		return EXIT_FILE_ERR;

	inbuf = NULL;
	inbuf_sz = 0;

	outbuf = NULL;
	outbuf_sz = 0;

	if ( args->preserve ) {
		size_t str_len;

		if ( getline(&inbuf, &inbuf_sz, in) <= 0 ) {
			fprintf(stderr, "aggregate: unexpected end of file\n");
			exit(EXIT_FILE_ERR);
		}
		chomp(inbuf);

		outbuf = malloc( inbuf_sz );
		if ( ! outbuf ) {
			fprintf(stderr, "aggregate: out of memory.\n");
			exit(EXIT_MEM_ERR);
		}
		outbuf_sz = inbuf_sz;

		/* NOTE also that sums have to be printed out before counts
		   to maintain backwards compatibility with the old perl
		   version of aggregate.
		 */

		extract_fields_to_string (inbuf, outbuf, outbuf_sz,
					  key_fields, nkeys, delim ) ;

		if ( nsums ) {
			strcat(outbuf, delim);
			str_len = strlen(outbuf);
			extract_fields_to_string (inbuf, outbuf + str_len,
						  outbuf_sz - str_len,
						  sum_fields, nsums, delim ) ;
		}

		if ( ncounts ) {
			strcat(outbuf, delim);
			str_len = strlen(outbuf);
			extract_fields_to_string (inbuf, outbuf + str_len,
						  outbuf_sz - str_len,
						  count_fields, ncounts,
						  delim ) ;
		}

		puts(outbuf);

	}

	ht_init( &aggregations, 1024, NULL, (void (*)) free_agg );
	/* ht_init( &aggregations, 1024, NULL, free ); */

	n_hash_elems = 0;

	/* loop through all files */
	while ( in != NULL ) {
		struct aggregation *value;
		char tmpbuf[64];
		size_t tmplen;
		int in_hash;

		/* loop through each line of the file */
		while ( getline(&inbuf, &inbuf_sz, in) > 0 ) {
			chomp(inbuf);
			if ( inbuf_sz > outbuf_sz ) {
				char *tmp_outbuf = realloc( outbuf, inbuf_sz + 32 );
				if ( ! tmp_outbuf ) {
					fprintf(stderr, "aggregate: out of memory.\n");
					goto aggregate_cleanup;
				}
				outbuf = tmp_outbuf;
				outbuf_sz = inbuf_sz + 32;
			}

			extract_fields_to_string (inbuf, outbuf, outbuf_sz, key_fields, nkeys, delim ) ;

			value = (struct aggregation *) ht_get(&aggregations, outbuf);
			if ( ! value ) {
				in_hash = 0;
				value = alloc_agg( nsums , ncounts );
				/* value = malloc(sizeof(struct aggregation));
				   memset(value, 0, sizeof(struct aggregation)); */
				if ( ! value ) {
					fprintf(stderr, "aggregate: out of memory.\n");
					goto aggregate_cleanup;
				}
			} else {
				in_hash = 1;
			}

			for ( i = 0; i < nsums; i++ ) {
				tmplen = get_line_field( tmpbuf, inbuf, 63, sum_fields[i], delim );
				if ( tmplen > 0 ) {
					n = float_str_precision(tmpbuf);
					if ( sum_precisions[i] < n )
						sum_precisions[i] = n;
					value->sums[i] += atof(tmpbuf);
				}
			}

			for ( i = 0; i < ncounts; i++ ) {
				tmplen = get_line_field( tmpbuf, inbuf, 63, count_fields[i], delim );
				if ( tmplen > 0 ) {
					value->counts[i] += 1;
				}
			}

			if ( ! in_hash ) {
				if ( ht_put(&aggregations, outbuf, value) != 0 )
					fprintf(stderr, "failed to store value in hashtable.\n");
				n_hash_elems++;
			}

		}
		fclose(in);
		in = nextfile( argc, argv, &optind, "r" );
	}

	/* print all of the output */
	if ( args->nosort ) {
		/* it will be a little faster if the user indicates that
		   the output sort order doesn't matter */
		for ( i = 0; i < aggregations.arrsz; i++ ) {
			hash_keys = aggregations.arr[i];
			if ( hash_keys != NULL ) {
				ll_call_for_each ( hash_keys, ht_print_keys_sums_counts );
			}
		}
	} else {
		llist_node_t *node;
		struct aggregation *val;
		char **key_array;
		int j = 0;
		key_array = malloc(sizeof(char*) * n_hash_elems);

		/* put all the keys into an array */
		for ( i = 0; i < aggregations.arrsz; i++ ) {
			hash_keys = aggregations.arr[i];
			if ( hash_keys != NULL ) {
				for ( node = hash_keys->head; node; node = node->next ) {
					key_array[j] = ((ht_elem_t *) node->data)->key;
					j++;
				}
			}
		}

		/* sort the keys */
		qsort( key_array, n_hash_elems, sizeof(char*),
			(int (*)(const void *, const void *))key_strcmp );
			/* (int (*)(const void *, const void *))strcmp ); */

		/* print everything out */
		for ( i = 0; i < n_hash_elems; i++ ) {
			val = (struct aggregation *) ht_get(&aggregations, key_array[i]);
			print_keys_sums_counts ( key_array[i], val );
		}

		free(key_array);
	}


aggregate_cleanup:
	ht_destroy(&aggregations);

	if ( key_fields )
		free(key_fields);
	if ( sum_fields )
		free(sum_fields);
	if ( count_fields )
		free(count_fields);
	if ( inbuf )
		free(inbuf);
	if ( outbuf )
		free(outbuf);

	return EXIT_OKAY;
}

int key_strcmp ( char **a, char **b ) {
	char fa[256], fb[256];
	int retval = 0;
	int i;
	size_t alen, blen;

	/* avoid comparing nulls */
	if ( ! *a && ! *b )
		return 0;
	if ( ! *a && *b )
		return -1;
	if ( *a && ! *b )
		return 1;

	i = 0;
	while ( retval == 0 ) {
		alen = get_line_field(fa, *a, 255, i, delim);
		blen = get_line_field(fb, *b, 255, i, delim);
		if ( alen < 0 || blen < 0 )
			break;
		retval = strcmp(fa, fb);
		i++;
	}

	return retval;
}

int float_str_precision ( char *d ) {
	char *p;
	int after_dot;
	if ( ! d )
		return 0;

	p = strchr(d, '.');
	if ( p == NULL ) {
		return 0;
	}
	after_dot = p - d + 1;
	return ( strlen(d) - after_dot );
}

int print_keys_sums_counts ( char * key, struct aggregation * val ) {
	int i;
	fputs(key, stdout);
	for ( i = 0; i < nsums; i++ ) {
		printf ( "%s%.*f", delim, sum_precisions[i], val->sums[i] );
	}
	for ( i = 0; i < ncounts; i++ ) {
		printf ( "%s%d", delim, val->counts[i] );
	}
	fputs("\n", stdout);
	return 0;
}

int ht_print_keys_sums_counts ( void * htelem ) {
	char *key;
	struct aggregation *val;
	key = ((ht_elem_t *) htelem)->key;
	val = ((ht_elem_t *) htelem)->data;
	return print_keys_sums_counts ( key, val );
}

void extract_fields_to_string (char *line, char *destbuf, size_t destbuf_sz, int *fields, size_t nfields, char *delim ) {
	char *pos;
	int i;
	size_t delim_len, field_len;

	delim_len = strlen(delim);
	pos = destbuf;

	for ( i = 0; i < nfields; i++ ) {
		field_len = get_line_field(pos, line, destbuf_sz - (pos - destbuf), fields[i], delim);
		pos += field_len;
		if ( i != nfields - 1 ) {
			strcat(pos, delim);
			pos += delim_len;
		}
	}
}

void decrement_values ( int *array, size_t sz ) {
	int j;
	if ( array == NULL || sz == 0 )
		return;
	for ( j = 0; j < sz; j++ ) {
		array[j]--;
	}
}

struct aggregation * alloc_agg ( int nsum, int ncount ) {
	struct aggregation *agg;

	agg = malloc(sizeof(struct aggregation));
	if ( ! agg )
		goto alloc_agg_error;

	agg->counts = NULL;
	agg->sums = NULL;

	if ( nsum > 0 ) {
		agg->sums = malloc( sizeof(double) * nsum );
		if ( ! agg->sums )
			goto alloc_agg_error;
		memset(agg->sums, 0, sizeof(double) * nsum);
	}

	if ( ncount > 0 )  {
		agg->counts = malloc( sizeof(u_int32_t) * ncount );
		if ( ! agg->counts )
			goto alloc_agg_error;
		memset(agg->counts, 0, sizeof(u_int32_t) * ncount);
	}

	return agg;

alloc_agg_error:
	if ( agg ) {
		if ( agg->sums )
			free(agg->sums);
		if ( agg->counts )
			free(agg->counts);
		free(agg);
	}
	return NULL;
}

void free_agg( struct aggregation *agg ) {
	if ( ! agg )
		return;
	if ( agg->counts )
		free(agg->counts);
	if ( agg->sums )
		free(agg->sums);
	free(agg);
}

