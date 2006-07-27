#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#ifndef INBUFFER_MAX
#define INBUFFER_MAX 2048
#endif

#ifndef MAX_TRUNC_FIELDS 
#define MAX_TRUNC_FIELDS 64
#endif

/*****************
 author: jhinds

 truncates one or more fields from a character-delimited file.

 options: 

-h|--help               print help message and exit
-d|--delimiter          single-character file delimiter
-f|--field              ordered comma-separated positive numeric field numbers
-o|--output-file        optional file for printing output.
-a|--append-file        optional file for appending output.

 cmd-line options are followed by optional list of input files.
 required options are delimiter and field.   default output is stdout,
 and stdin is used if no input files are specified.

 2004.01.22 with multi-field support.  see single_field_version directory for 
 previous version

 *****************/

/*** benchmarking

--- multi-field version ---
bash-2.03$ time ./truncfield -f 1,3,4 -d `tochar 0254` -o tf.out sa_event_886646_01-13-2004.log 
 
real    0m40.747s
user    0m20.690s
sys     0m19.400s

--- single-field version ---
bash-2.03$ time ./truncfield -d `tochar 0254` -f 1 sa_event_886646_01-13-2004.log | ./truncfield -d `tochar 0254` -f 3 | ./truncfield -d `tochar 0254` -f 4 -o tf.out 
 
real    1m16.536s
user    0m50.900s
sys     0m32.080s


***/

/* command-line option structure */
static struct option long_options[] =
        {
          /* These options set a flag. */
          {"help",	no_argument,	    0, 'h'},
          {"delimiter",	required_argument,  0, 'd'},
          {"field",	required_argument,  0, 'f'},
          {"output-file",required_argument, 0, 'o'},
          {"append-file",required_argument, 0, 'a'},
          {0, 0, 0, 0}
        };

/* output usage info to user */
void usage(char *);

/* splits comma-separated string of field numbers into an array of ints */
size_t set_fields(int*,char*);


int main(int argc, char *argv[]){
	FILE *in = NULL, *out = NULL;	/* input and output file pointers */
	unsigned char delim='\0';	/* delimiter character */

	/* buffer holds one line of input
	   pos points at POSition of current delimiter
	   ppos points at Previous POSition of delimiter
	   mark points at the end of the last field that was truncated */
	char *buffer, *pos, *ppos, *mark;

	int inctr = 0;	/* current input file number */

	/* fields = array of field indeces to be truncated
	   n_fields = number of elements in array */
	int fields[MAX_TRUNC_FIELDS], n_fields = 0;

	/* vars for option parsing */
	int c, option_index;

	while( (c = getopt_long(argc, argv, "hd:f:a:o:", long_options,
				&option_index) ) != -1)
	switch(c){
		case 'h':
			usage(argv[0]);
			exit(1);
			break;
		case 'd':
			delim = optarg[0];
			break;
		case 'o':
			if(out == NULL)
				if(! (out = fopen(optarg, "w")) ){
					perror(optarg);
					exit(-1);
				}
			break;
		case 'a':
			if(out == NULL)
				if(! (out = fopen(optarg, "a")) ){
					perror(optarg);
					exit(-1);
				}
			break;
		case 'f':
			/* sscanf(optarg, "%u", &field); */
			n_fields = set_fields(fields, optarg);
			break;
		case '?':	/* getopt_long prints error message */
			break;
		default:
			if(out) fclose(out);
			exit(-1);
			break;
	}

	/* terminate if no fields or delimiter are specified */
	if(delim == '\0' || n_fields == 0){
		usage(argv[0]);
		if(out != NULL) fclose(out);
		exit(1);
	}

	/* for testing purposes */
#ifdef DEBUG
	fprintf(stderr,"%d fields specified.\n",n_fields);
#endif

	if(out == NULL)
		out = stdout;

	if( (buffer = malloc(INBUFFER_MAX)) == NULL){
		fprintf(stderr,"unable to allocate memory.\n");
		fclose(out);
		return -1;
	}

	inctr = optind;	/* optind comes from getopt */

	/* loop through input files */
	do{
		int i, fctr = 0;	/* i = current found field
					   fctr = truncated field counter,
					   	works as index into fields[] */

		/* use stdin if no input files are specified */
		if(inctr == argc){
			in = stdin;
		}
		else if( (in = fopen( argv[inctr], "r")) == NULL ){
			perror(argv[inctr]);
			fclose(out);
			free(buffer);
			return 2;
		}
		while(fgets(buffer, INBUFFER_MAX, in) != NULL) {
			i = 0;
			fctr = 0;
			mark = ppos = buffer;
			while( (pos = strchr(ppos, delim)) != NULL ){
				i++;
				if(i == fields[fctr]){
					fwrite(mark, sizeof(char),
							ppos - mark, out);
					if(++fctr == n_fields){
						fputs(pos, out);
						break;
					}
					mark = pos ;
				}
				ppos = pos +1 ;
			}
			/* handle the last field */
			if(fctr != n_fields && ++i == fields[fctr]){
				fwrite(mark, sizeof(char),
					ppos - mark, out);
				fputc('\n',out);
			}
			/* handle lines w/out as many fields as specified */
			else if(fctr < n_fields && i<fields[fctr])
				fputs(mark, out);
		}

		fclose(in);
	} while ( ++inctr < argc );

	free(buffer);
        fclose(out);

	return 0;
}


/****
 * splits comma-separated string into array of ints
 *
 * param - int *fields - array to store field numbers
 * param - char *arg - string of fields provided on cmd-line
 *
 * returns number of elements in array
 ****/
size_t set_fields(int *fields, char *arg){
	int i = 0, o = 0;
	/* token = string of current field number
	   pos = POSition of current comma
	   ppos = Previous POSition of comma */
	char token[9], *pos, *ppos;

	/* don't bother if no fields are provided */
	if(arg == NULL || strlen(arg) == 0){
		return 0;
	}
	/* if only one field was provided, don't worry about tokenizing */
	if( strchr(arg,',') == NULL){
		sscanf(arg,"%u",&(fields[0]));
		return 1;
	}
	ppos = arg;
	/* tokenize arg, but don't overflow the array */
	while( (pos = strchr(ppos, ',')) != NULL && i < MAX_TRUNC_FIELDS ){
		o = pos - ppos;
		strncpy(token, ppos, o);
		token[o] = '\0';
		sscanf(token,"%u",&(fields[i]));
		ppos = pos + 1;
		i++;
	}
	if(i < MAX_TRUNC_FIELDS){
		/* get the last token */
		o = strlen(ppos);
		strncpy(token, ppos, o);
		token[o] = '\0';
		sscanf(token,"%u",&(fields[i]));
		i++;
	}
	return i;
}


/*
   print usage message.
   bin = name of executed binary (argv[0])
*/
void usage(char *bin){
	fprintf(stderr,"2004, jhinds\n");
	fprintf(stderr,"\ntruncates fields from a character-delimited file.\n");
	fprintf(stderr,
	"\nusage: %s <-d delim> <-f field> [ -o|-a outfile ] [input file(s)]\n\n", bin);
	fprintf(stderr,"-h|--help\t\tprint this message and exit\n");
	fprintf(stderr,"-d|--delimiter\t\tsingle-character file delimiter\n");
	fprintf(stderr,"-f|--field\t\tordered comma-separated positive numeric field numbers\n");
	fprintf(stderr,"-o|--output-file\toptional file for printing output.\n");
	fprintf(stderr,"-a|--append-file\toptional file for appending output.\n\n");
	fprintf(stderr,"you shouldn't use both -a and -o, but if you do, only the first will be used.\n\n");
}
