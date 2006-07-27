/********************************
  copyright
 ********************************/
#include "indexof_main.h"
#include "indexof.h"

int indexof ( struct cmdargs *args, int argc, char *argv[], int optind ){
	FILE *in;
	int lineno = 1, curline = 1, index = 1;
	char *buf = NULL, *tok;
	size_t buflen = 0;

	if( args->file != NULL ){
		if( (in = fopen(args->file, "r")) == NULL ){
			perror(args->file);
			return EXIT_FILE_ERR;
		}
	} else
		in = stdin;
	if( args->row != 0 )
		sscanf(args->row, "%d", &lineno);

	if( args->string == NULL ){
		do {
			if( getline( &buf, &buflen, in ) == -1 ){
				free(buf);
				fprintf(stderr, "out of memory\n");
				return EXIT_MEM_ERR;
			}
			curline++;
		} while ( lineno > curline && ! feof(in) );

		if( feof(in) ){
			fprintf(stderr, "invalid line number: %d\n", lineno);
			exit(EXIT_HELP);
		}
	} else {
		buf = malloc(strlen(args->string) + 1);
		if( buf == NULL ){
			fprintf(stderr, "out of memory\n");
			return EXIT_MEM_ERR;
		}
		strcpy(buf, args->string);
	}

	tok = strtok(buf, args->delim);
	while( tok != NULL && strcmp( args->label, tok ) != 0 ){
		tok = strtok(NULL, args->delim);
		index++;
	}
	if( tok == NULL )
		index = 0;

	free(buf);
	printf("%d\n", index);
	return EXIT_OKAY;
}

