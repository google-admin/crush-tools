/********************************
  copyright
 ********************************/
#include "mergekeys_main.h"
#include "mergekeys.h"

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
	int *mergefields;		/* fields only in file b - need to be added
					   to those in a */
	size_t nkeys, ntomerge;
	int keycmp = 0;
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

	if( (keyfields = malloc(sizeof(int) * nfields_a)) == NULL ) {
		fprintf(stderr, "allocating key array - out of memory\n");
		free(bufa);
		free(bufb);
		return EXIT_MEM_ERR;
	}
	if( (mergefields = malloc(sizeof(int) * nfields_b)) == NULL ) {
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

	/* if b had more fields than a, all of those need merged */
	for( ; i < nfields_b; i++ ) {
		mergefields[ ntomerge++ ] = i;
	}

	/* print the headers which were already read in above */
	fputs(bufa, out);
	for ( i = 0; i < ntomerge; i++ ) {
		get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
		fprintf(out, "%s%s", args->delim, field_b);
	}
	fputc('\n', out);

	/* go through the rest of the 2 files */
	while ( !feof(a) ) {
		if ( keycmp <= 0 ) {
			/* keys were either the same for previous line,
			 * or keys from a were smaller (a printed out).
			 * need another line from a
			 */
			if ( getline(&bufa, &basz, a) <= 0 ) {
				break;
			}
			bufa_flag = 1;
			chomp(bufa);
		}
		if ( keycmp >= 0 ) {
			/* keys were either the same for previous line,
			 * or keys from b were smaller (b printed out).
			 * need another line from b
			 */
			if ( getline(&bufb, &bbsz, b) > 0 )
			{
				chomp(bufb);
				bufb_flag = 1;
			}
		}

		if ( !feof(b) ) {

			/** @todo make sure the key fields are the same here */
			for ( i = 0; i < nkeys; i++ ) {
				get_line_field(field_a, bufa, MAX_FIELD_LEN, keyfields[i], args->delim);
				get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[i], args->delim);
				if ( (keycmp = strcmp( field_a, field_b ) ) != 0 )
					break;
			}

			if ( keycmp == 0 ) {
				/* keys are equal */
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}
				bufa_flag = 0;
				bufb_flag = 0;

				fputc('\n', out);
			} else if ( keycmp < 0 ) {
				/* key from a is less than key from b:
				 * print line from a with empty b fields
				 * and continue
				 */
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				bufa_flag = 0;

				fputc('\n', out);
			} else if ( ! args->left ) {
				/* key from a is greater than key from b:
				 * print b fields with empty a fields
				 * and continue
				 */
				int j = 0;
				/* print the keys from b */
				for ( i = 0; i < nfields_a; i++ ) {
					if ( keyfields[j] == i ) {
						get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[j], args->delim);
						fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_b);
						j++;
					} else {
						fputs(args->delim, out);
					}
				}

				/* print the non-keys from b */
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}

				bufb_flag = 0;

				fputc('\n', out);
			}
		} else {
			/* no more lines in file b - just print empty fields */
			fprintf(out, "%s", bufa);
			for ( i = 0; i < ntomerge; i++ )
				fputs(args->delim, out);
			keycmp = -1;
			bufa_flag = 0;

			fputc('\n', out);
		}
	}

	/* if there's anything left in file b, print it out with empty fields for a */
	if ( ! feof(b) ) {


		/* If the last line of file a is already written, make sure that it will not be touched again. */
		if (bufa_flag == 0)
			keycmp = 1;

		while (! feof(b) )
		{
			/* Do not read the next line until the last "b" line of the first loop is written. */
			if (bufb_flag == 0)
			{
				if ( getline(&bufb, &bbsz, b) > 0 )
					chomp(bufb);
				else
					break;
			}

			/* make sure the last key from file a was printed */
			if ( keycmp < 0 ) {
				for ( i = 0; i < nkeys; i++ ) {
					get_line_field(field_a, bufa, MAX_FIELD_LEN, keyfields[i], args->delim);
					get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[i], args->delim);
					if ( (keycmp = strcmp( field_a, field_b ) ) != 0 )
						break;
				}
			}

			if ( keycmp == 0 ) {
				/* keys are equal */
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}
				keycmp = 1;

				fputc('\n', out);
			} else if ( keycmp < 0 ) {

				/* key from a is less than key from b:
				 * print line from a with empty b fields
				 */
				fprintf(out, "%s", bufa);
				for ( i = 0; i < ntomerge; i++ )
					fputs(args->delim, out);
				fputc('\n', out);

				keycmp = 1;

				/* now print b also */
				if( ! args->left ) {
					int j = 0;
					/* print the keys from b */
					for ( i = 0; i < nfields_a; i++ ) {
						if ( keyfields[j] == i ) {
							get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[j], args->delim);
							fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_b);
							j++;
						} else {
							fputs(args->delim, out);
						}
					}

					/* print the non-keys from b */
					for ( i = 0; i < ntomerge; i++ ) {
						get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
						fprintf(out, "%s%s", args->delim, field_b);
					}

					fputc('\n', out);
				}
			} else if ( ! args->left ) {
				/* key from a is greater than key from b:
				 * print b fields with empty a fields
				 * and continue
				 */
				int j = 0;
				/* print the keys from b */
				for ( i = 0; i < nfields_a; i++ ) {
					if ( keyfields[j] == i ) {
						get_line_field(field_b, bufb, MAX_FIELD_LEN, keyfields[j], args->delim);
						fprintf(out, "%s%s", (j > 0 ? args->delim : ""), field_b);
						j++;
					} else {
						fputs(args->delim, out);
					}
				}

				/* print the non-keys from b */
				for ( i = 0; i < ntomerge; i++ ) {
					get_line_field(field_b, bufb, MAX_FIELD_LEN, mergefields[i], args->delim);
					fprintf(out, "%s%s", args->delim, field_b);
				}

				fputc('\n', out);
			}

			bufb_flag = 0;
		}
	}

	free(bufa);
	free(bufb);
	free(keyfields);
	free(mergefields);
	return EXIT_OKAY;
}

