/********************************
  copyright
 ********************************/
#include "mergekeys_main.h"
#include "mergekeys.h"
#include <locale.h>


/* macros to clarify the semantics of key comparisons. */

/* these are used to compare "keycmp" */
#define A_LT_B(n) (n) < 0
#define A_LE_B(n) (n) <= 0
#define A_EQ_B(n) (n) == 0
#define A_GT_B(n) (n) > 0
#define A_GE_B(n) (n) >= 0

/* these are used to assign to keycmp */
#define A_GREATER  1
#define B_GREATER -1
#define AB_EQUAL   0


/** @brief opens all the files necessary, sets a default
  * delimiter if none was specified, and calls the
  * merge_files() function.
  *
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  *
  * @return exit status for main() to return.
  */
int mergekeys ( struct cmdargs *args, int argc, char *argv[], int optind ){
	char default_delimiter[] = { 0xfe, 0x00 };
	FILE *a, *b, *out;	/* the two inputs & the output file ptrs */
	int fd_tmp, retval;	/* file descriptor and return value*/

	if ( argc - optind != 2 ) {
		fprintf(stderr, "missing file arguments.  see %s -h for usage information.\n", argv[0]);
		return EXIT_HELP;
	}

	if( (fd_tmp = open64(argv[optind], O_RDONLY)) < 0 ){
		perror(argv[optind]);
		return EXIT_FILE_ERR;
	}
	if ( (a = fdopen(fd_tmp, "r")) == NULL ) {
		perror(argv[optind]);
		return EXIT_FILE_ERR;
	}

	if( (fd_tmp = open64(argv[optind+1], O_RDONLY)) < 0 ){
		perror(argv[optind+1]);
		return EXIT_FILE_ERR;
	}
	if ( (b = fdopen(fd_tmp, "r")) == NULL ) {
		perror(argv[optind+1]);
		return EXIT_FILE_ERR;
	}

	if ( ! args->outfile )
		out = stdout;
	else {
		if( (fd_tmp = open64(args->outfile, O_WRONLY | O_CREAT | O_TRUNC)) < 0 ){
			perror(args->outfile);
			return EXIT_FILE_ERR;
		}
		fchmod(fd_tmp, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if ( (out = fdopen(fd_tmp, "w")) == NULL ) {
			perror(args->outfile);
			return EXIT_FILE_ERR;
		}
	}

	if ( ! args->delim ) {
		args->delim = getenv("DELIMITER");
		if ( ! args->delim )
			args->delim = default_delimiter;
	}
	expand_chars(args->delim);

	/* set locale with values from the environment so strcoll()
	   will work correctly. */
	setlocale(LC_ALL, "");
	setlocale(LC_COLLATE, "");

	retval = merge_files( a, b, out, args );

	fclose(a);
	fclose(b);
	fclose(out);

	return retval;
}

int merge_files( FILE *a, FILE *b, FILE *out, struct cmdargs *args ) {

	char *bufa, *bufb;		/* input line buffers for first & second files */
	ssize_t basz, bbsz;		/* size of the buffers */

	size_t nfields_a,		/* the number of fields in the first file */
	       nfields_b;		/* the number of fields in the second file */

	char field_a[MAX_FIELD_LEN+1],	/* buffers for holding field values */
	     field_b[MAX_FIELD_LEN+1];

	int i;				/* general-purpose counter */

	/** @todo take into account that files a & b might have the same fields in
	  * a different order.
	  */
	int *keyfields;			/* array of fields common to both files */
	int *mergefields;		/* fields only in file B - need to be added
					   to those in A */
	size_t nkeys, ntomerge;
	int keycmp = 0;
	int first_data_line = 1;
	int bufa_flag = 0;		/* 0 => there is no data line in bufx or the line in bufx has already been written.*/
	int bufb_flag = 0;		/* 1 => there is data in bufx which needs to be handled.*/


	bufa = bufb = NULL;
	basz = bbsz = 0;

	if ( getline(&bufa, &basz, a) <= 0 ) {
		fprintf(stderr, "no header found in first file\n");
		return EXIT_FILE_ERR;
	}
	if ( getline(&bufb, &bbsz, b) <= 0 ) {
		fprintf(stderr, "no header found in second file\n");
		return EXIT_FILE_ERR;
	}

	chomp(bufa);
	chomp(bufb);

	nfields_a = fields_in_line(bufa, args->delim);
	nfields_b = fields_in_line(bufb, args->delim);

	if ( args->verbose )
		printf("fields in a: %s\nfields in b: %s\n", bufa, bufb);

	if ( (keyfields = malloc(sizeof(int) * nfields_a)) == NULL ) {
		fprintf(stderr, "allocating key array - out of memory\n");
		free(bufa);
		free(bufb);
		return EXIT_MEM_ERR;
	}
	if ( (mergefields = malloc(sizeof(int) * nfields_b)) == NULL ) {
		fprintf(stderr, "allocating merge-fields array - out of memory\n");
		free(bufa);
		free(bufb);
		free(keyfields);
		return EXIT_MEM_ERR;
	}

	nkeys = ntomerge = 0;
	/* find the keys common to both files & the ones that need merged */
	for ( i = 0; i < (nfields_a < nfields_b ? nfields_a : nfields_b); i++ ) {
		get_line_field(field_a, bufa, MAX_FIELD_LEN, i, args->delim);
		get_line_field(field_b, bufb, MAX_FIELD_LEN, i, args->delim);
		if ( str_eq(field_a, field_b) )
			keyfields[ nkeys++ ] = i;
		else
			mergefields[ ntomerge++ ] = i;
	}

	/* if B had more fields than A, all of those need merged */
	for ( ; i < nfields_b; i++ ) {
		mergefields[ ntomerge++ ] = i;
	}

	/* print the headers which were already read in above */
	fputs(bufa, out);
	for ( i = 0; i < ntomerge; i++ ) {
		get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
		fprintf(out, "%s%s", args->delim, field_b);
	}
	fputc('\n', out);

	/* this handles the case where A has only a header, forcing a
	   line-read at the beginning of the section that deals with
	   the tail end of file B. */
	keycmp = A_GREATER;
	bufb_flag = 0;

	/* go through the rest of the 2 files */
	while ( !feof(a) ) {
		/* if keys are equal, there is a chance that the next line of file B will
		 * match the keys from A as well.  so keep the current line from file A.
		 */
		if ( first_data_line || A_LT_B( keycmp ) ) {
			/* keys from A were smaller (A printed out).
			 * need another line from A
			 */
			if ( getline(&bufa, &basz, a) <= 0 ) {
				/* EOF. make sure we don't print any A fields while
				   handling the rest of file B.
				 */
				keycmp = B_GREATER;
				break;
			}
			bufa_flag = 1;
			chomp(bufa);
		}
		if ( first_data_line || A_GE_B( keycmp ) ) {
			/* keys were either the same for previous line,
			 * or keys from B were smaller (B printed out).
			 * need another line from B
			 */
			if ( getline(&bufb, &bbsz, b) > 0 )
			{
				chomp(bufb);
				bufb_flag = 1;
			}
		}

		first_data_line = 0;

		if ( !feof(b) ) {

			/** @todo make sure the key fields are the same here */
			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_a, bufa, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcoll( field_a, field_b ) ) != 0 )
					break;
			}

			if ( args->left && A_GT_B( keycmp ) ) {
				/* no match in B - don't print this line from A */
				bufb_flag = 0;
				continue;
			}

			/* if line B is greater than line A, and A has already been
			   printed once, just get another line from A
			 */
			if ( A_LT_B( keycmp ) && bufa_flag == 0 )
				continue;

			if ( A_EQ_B( keycmp ) ) {
				/* keys are equal */
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}
				bufa_flag = 0;
				bufb_flag = 0;

				fputc('\n', out);
			}
			else if ( A_LT_B( keycmp ) ) {
				/* key from A is less than key from B:
				 * print line from A with empty B fields
				 * and continue
				 */
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				bufa_flag = 0;

				fputc('\n', out);
			}
			else {
				/* key from A is greater than key from B:
				 * print B fields with empty A fields
				 * and continue
				 */
				int j = 0;
				/* print the keys from B */
				for ( i = 0; i < nfields_a; i++ ) {
					if ( keyfields[j] == i ) {
						get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[j], args->delim);
						fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_b);
						j++;
					} else {
						fputs(args->delim, out);
					}
				}

				/* print the non-keys from B */
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}

				bufb_flag = 0;

				fputc('\n', out);
			}
		}
		else {	/* feof(b) */

			/* no more lines in file B - if line from A hasn't been printed yet,
			   just print it with empty B fields and force the comparison 
			   result to show B as greater than A.
			 */

			if ( bufa_flag == 1 ) {
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				fputc('\n', out);
			}
			keycmp = B_GREATER;
			bufa_flag = 0;
			bufb_flag = 0;

		}
	}

	/* if there's anything left in file B, print it out with empty fields for A */
	if ( ! feof(b) ) {

		/* Handle all lines from B with keys less than last line of A */
		while ( A_GT_B( keycmp ) && ! feof(b) ) {
			int j = 0;
			if ( bufb_flag == 0 ) {
				if ( getline(&bufb, &bbsz, b) <= 0 )
					break;
				chomp(bufb);
				bufb_flag = 1;
			}

			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_a, bufa, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcoll( field_a, field_b ) ) != 0 )
					break;
			}

			if ( ! A_GT_B( keycmp ) ) {
				break;
			}

			if ( args->left ) {
				/* skip non-matching lines */
				bufb_flag = 0;
				continue;
			}

			/* print the keys from B */
			for ( i = 0; i < nfields_a; i++ ) {
				if ( keyfields[j] == i ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[j], args->delim);
					fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_b);
					j++;
				} else {
					fputs(args->delim, out);
				}
			}

			/* print the non-keys from B */
			for ( i = 0; i < ntomerge; i++ ) {
				get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
				fprintf(out, "%s%s", args->delim, field_b);
			}

			fputc('\n', out);

			bufb_flag = 0;
		}

		/* Handle all lines from B with keys equal to the last line of A */
		while ( A_EQ_B( keycmp ) && ! feof(b) ) {
			if ( bufb_flag == 0 ) {
				if ( getline(&bufb, &bbsz, b) <= 0 )
					break;
				chomp(bufb);
				bufb_flag = 1;
			}

			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_a, bufa, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcoll( field_a, field_b ) ) != 0 )
					break;
			}

			if ( ! A_EQ_B( keycmp ) ) {
				break;
			}

			fprintf(out, "%s", bufa);
			for ( i = 0; i < ntomerge; i++ ) {
				get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
				fprintf(out, "%s%s", args->delim, field_b);
			}

			fputc('\n', out);

			bufa_flag = 0;
			bufb_flag = 0;
		}

		/* If a match between A and B is required (i.e. args->left),
		   we can safely skip the rest of file B. */
		if ( A_LT_B( keycmp ) && ! args->left ) {

			/* if the last line from A hasn't been printed yet, and no match was found
			   in B, print A line now with empty B fields. */
			if ( bufa_flag != 0 ) {
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				fputc('\n', out);
			}

			/* The last line of A has definitely now been printed. dump the rest of
			   file B with empties for non-key A fields */
			while (! feof(b) ) {
				int j = 0;

				if ( bufb_flag == 0 ) {
					if ( getline(&bufb, &bbsz, b) <= 0 )
						break;
					chomp(bufb);
					bufb_flag = 1;
				}

				/* print the keys from B */
				for ( i = 0; i < nfields_a; i++ ) {
					if ( keyfields[j] == i ) {
						get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[j], args->delim);
						fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_b);
						j++;
					} else {
						fputs(args->delim, out);
					}
				}

				/* print the non-keys from B */
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}

				fputc('\n', out);
				bufb_flag = 0;
			}
		}

	}

	free(bufa);
	free(bufb);
	free(keyfields);
	free(mergefields);
	return EXIT_OKAY;
}

