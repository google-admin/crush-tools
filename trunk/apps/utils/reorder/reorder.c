/********************************
  copyright
 ********************************/
#include "reorder_main.h"
#include "reorder.h"

size_t n_swap = 0;

/* TODO: get rid of arbitrary limit on number of swaps */
int swap[32][3];

/** @brief copies ct into s, swapping fields in the process.
  * 
  * @param s destination buffer
  * @param ct source buffer
  * @param d delimiter
  */
void doswap( char *s, char *ct, const char *d );

/** @brief creates in s a new string delimited by d, containing the fields
  * from ct in the order specified by o.
  *
  * because a field may be included multiple times in the destination buffer,
  * it is entirely possible for the output to be larger than the input, so
  * this function may dynamically resize s as necessary.
  *
  * @param s address of destination buffer
  * @param ct source buffer
  * @param s_sz address of the size of destination buffer
  * @param d delimiter
  * @param o array of field numbers
  * @param n number of elements in o
  *
  * @return the length of s on success; < 0 on error
  */
int docut( char **s, const char *ct, size_t *s_sz, const char *d, const int *order, const size_t n );

int reorder ( struct cmdargs *args, int argc, char *argv[], int optind ){
	FILE *fp, *fpout;
	char *lbuf = NULL;	/* line buffer */
	size_t lbs = 0;		/* line buffer size */

	char *wbuf = NULL;	/* working buffer */
	size_t wbs = 0;		/* working buffer size */

	int *order = NULL;
	size_t order_sz = 0;
	size_t order_elems = 0;

	char default_delim[] = { 0xfe, 0x00 };

#ifdef DEBUG
	int i;
	for( i = 0; i < n_swap; i++ ) {
		if ( swap[i][2] == REORDER_TYPE_SWAP )
			printf("swap %d with %d\n", swap[i][0], swap[i][1]);
		else
			printf("move %d to %d\n", swap[i][0], swap[i][1]);
	}
#endif

	if ( ! args->delim ) {
		args->delim = default_delim;
	}

	/* may add output option later */
	fpout = stdout;

	if ( optind == argc )
		fp = stdin;
	else
		fp = nextfile ( argc, argv, &optind, "r" );

	if( fp == NULL )
		return EXIT_FILE_ERR;

	if ( args->fields ) {

		order_sz = fields_in_line(args->fields, ",");
		if ( (order = malloc(order_sz * sizeof(int))) == NULL )
			goto memerror;

		order_elems = splitnums(args->fields, &order, order_sz);

		if ( args->verbose ) {
			int i;
			fprintf(stderr, "there are %d fields: ", order_sz);
			for ( i = 0; i < order_elems; i++ ) {
				fprintf(stderr, " %d", order[i]);
			}
			fprintf(stderr, "\n");
		}
	}

	do {
		while( 1 ){

			if( getline(&lbuf, &lbs, fp) < 0 )
				break;

			/* make sure there's enough room in the working
			   buffer */
			if( wbuf == NULL ){
				if( (wbuf = malloc(lbs)) == NULL )
					goto memerror;
				wbs = lbs;
			} else if ( wbs < lbs ) {
				/* if realloc unsuccessful, we don't want
				 * wbuf to end up being NULL
				 */
				char *tmp_ptr;
				if( ( tmp_ptr = realloc(wbuf, lbs)) == NULL )
					goto memerror;
				wbuf = tmp_ptr;
				wbs = lbs;
			}

			if ( ! args->fields ) {
				doswap( wbuf, lbuf, args->delim );
			} else {
				if ( docut( &wbuf, lbuf, &wbs, args->delim, order, order_elems ) < 0 ) 
					goto memerror;
			}
			fprintf(fpout, "%s", wbuf);
			memset( lbuf, 0, lbs );
			memset( wbuf, 0, wbs );
			/* fputs(lbuf, fpout); */

		}

		fclose(fp);
		fp = NULL;

		optind++;

		if( optind >= argc )
			break;

		fp = nextfile ( argc, argv, &optind, "r" );

		if (fp == NULL)
			break;

	} while ( optind <= argc );

	fflush(fpout);
	fclose(fpout);

	if ( order )
		free(order);
	free(lbuf);
	free(wbuf);

	return EXIT_OKAY;

	/* cleanup stuff in case of an error with malloc or realloc */
memerror:
	fprintf(stderr, "out of memory.\n");
	if ( order )
		free(order);
	if ( lbuf )
		free(lbuf);
	if ( wbuf )
		free(wbuf);
	if ( fp )
		fclose(fp);
	if ( fpout ) {
		fflush(fpout);
		fclose(fpout);
	}
	return EXIT_MEM_ERR;
}

void doswap( char *s, char *ct, const char *d ){
	int i;

	/* beginning and end positions for fields a and b */
	char *as, *ae, *bs, *be;

	size_t sl, dl;		/* string and delimiter length */
	size_t num_fields;	/* the number of fields in the line */

	sl = strlen(ct);
	dl = strlen(d);
	num_fields = fields_in_line( ct, d );

	chomp(ct);

	for( i = 0; i < n_swap; i++ ){
		if ( swap[i][0] > num_fields || swap[i][1] > num_fields )
			continue;

		memset(s, 0, sl+1);
		as = bs = ct;

		as = field_start(ct, (size_t) swap[i][0], d);
		assert( as != NULL );

		bs = field_start(ct, (size_t) swap[i][1], d);
		assert( bs != NULL );

		ae = strstr(as, d);
		be = strstr(bs, d);

		/* end of A is end of string - should never happen for swaps */
		if( ae == NULL && swap[i][2] == REORDER_TYPE_SWAP )
			continue;
		else if ( ae != NULL && swap[i][2] == REORDER_TYPE_MOVE )
			ae += dl;
		else if ( ae == NULL )
			ae = ct + sl;

		/* end of B is end of string - B is last field */
		if( be == NULL ) be = ct + sl;
		else if ( swap[i][2] == REORDER_TYPE_MOVE )
			be += dl;

#ifdef DEBUG
		fprintf(stderr, "doswap: beginning of A is at %d, A is %d long\n", as - ct, ae - as);
		fprintf(stderr, "doswap: beginning of B is at %d, B is %d long\n", bs - ct, be - bs);
#endif

		/* note: the use of strncpy() below is safe because
		   s has been nulled out above */
		if ( swap[i][2] == REORDER_TYPE_SWAP ) {
			/* A is lower-indexed field, B is higher-indexed field */
			
			strncpy(s, ct, as - ct);	/* copy up to field A */
			strncat(s, bs, be - bs );	/* append field B */
			strncat(s, ae, bs - ae );	/* append everything between end of A and beginning of B */
			strncat(s, as, ae - as );	/* append field A */
			strcat(s, be);			/* append everything after b */

		} else if ( swap[i][2] == REORDER_TYPE_MOVE ) {

			/* A is field to be moved, B is destination field */

			if ( bs > as ) {			/* ---- moving A toward end of line ---- */

				strncpy(s, ct, as - ct);	/* copy up to field A */

				strncat(s, ae, be - ae );	/* append everything between end of A and end of B */

				if ( be == ct + sl ) {		/* if putting A on the end of the string, */
					strcat( s, d );		/*	append a delimiter to the dest. string first */
					as[ ae-as-dl ] = 0x00;	/*	drop the delimiter at the end of A */
				}
				strncat(s, as, ae - as );	/* append field A */
				strncat(s, be, sl - (be - ct));	/* append from end of B to the end of line. */

			} else {				/* ---- moving A toward beginning of line ---- */

				strncpy(s, ct, bs - ct);	/* copy up to field B */

				strncat(s, as, ae - as );	/* append field A */
				if ( ae == ct + sl ) {		/* if moving A from the end of the line */
					strcat( s, d );		/*	append a delimiter to end of line */
					be -= dl;		/*	move end of B before the delimiter */
					*(as - dl) = 0x00;	/*	remove the delimiter before a */
				}

				strncat(s, bs, as - bs );	/* append from start of B to the start of A. */
				if ( ae != ct + sl )		/* if A was the last field, no need to copy stuff after it */
					strcat(s, ae);
			}

		} /* end REORDER_TYPE_MOVE */

		/* copy the modified version into original */
		strcpy(ct, s);

	} /* end for loop through all swap/move field pairs */

	strcat(ct, "\n");
	strcat( s, "\n");
}



int docut( char **s, const char *ct, size_t *s_sz, const char *d, const int *order, const size_t n ){
	int i;
	char buffer[512];
	size_t s_len;
	size_t buf_len;
	size_t delim_len;

	/* make sure the destination buffer is allocated */
	if ( *s == NULL || *s_sz == 0 ) {
		if ( (*s = malloc(strlen(ct))) == NULL ) {
			*s_sz = 0;
			return -1;
		}
		*s_sz = strlen(ct);
	}

	(*s)[0] = '\0';
	s_len = 0;
	delim_len = strlen(d);

	for ( i = 0; i < n; i++ ) {

		if ( (buf_len = get_line_field(buffer, ct, 511,
					order[i] - 1, (char *) d) ) >= 0 ) {

			if ( *s_sz < s_len + buf_len + delim_len ) {
				char *tmp;
				/* include room for a null terminator and line break.*/
				tmp = realloc(*s, *s_sz + buf_len + delim_len + 2);
				if ( tmp == NULL )
					return -1;
				*s = tmp;
				*s_sz += buf_len + delim_len + 2;
			}

			chomp(buffer);
			strcat(*s, buffer);
			s_len += buf_len;

			if ( i < n - 1 ) {
				strcat(*s, d);
				s_len += delim_len;
			}
		}
	}
	strcat(*s, "\n");

	return s_len + 1;
}


/* validates swap arg from commandline and pushes values into the array
   of swaps to be performed */
int pushswap(char *s, int action_type){
	int ret = 0;
	regex_t s_re;
	unsigned int a, b;
	char errbuf[1024];

	/* better if s_re could be static & compiled just once, but
	   performance here isn't all that critical anyway */

	/* compile the expression to match N,N */
	if( (ret = regcomp(&s_re, "^[0-9]+,[0-9]+$", REG_EXTENDED) ) != 0 ){
		regerror(ret, NULL, errbuf, 1024);
		fprintf(stderr, "%s\n", errbuf);
		return ret;
	}

	/* execute the expression */
	if( (ret = regexec(&s_re, s, 0, NULL, 0)) != 0 ){
		regerror(ret, &s_re, errbuf, 1024);
		fprintf(stderr, "%s\n", errbuf);
		regfree(&s_re);
		return ret;
	}
	regfree(&s_re);
	sscanf(s, "%d,%d", &a, &b);

	/* don't bother with the idiocy of swapping equal fields */
	if( a == b )
		return ret;

	/* TODO: get rid of arbitrary limit on number of swaps */
	if( n_swap < 32 ){

		/* if swapping, go ahead & order these so as
		   not to have to worry about it later.  order must
		   be preserved for moving */
		if( action_type == REORDER_TYPE_MOVE || a < b ){
			swap[n_swap][0] = a;
			swap[n_swap][1] = b;
		} else {
			swap[n_swap][0] = b;
			swap[n_swap][1] = a;
		}
		swap[n_swap][2] = action_type;
		n_swap++;
	}
	return ret;
}
