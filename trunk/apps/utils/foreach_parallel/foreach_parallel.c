/********************************
  copyright
 ********************************/
#include "foreach_parallel_main.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>

#define DEFAULT_MAX_PROCS 10

#ifdef DEBUG
#define DEBUG_PRINT(s) \
	fprintf(stderr, "%s" "\n", (s))
#else
#define DEBUG_PRINT(s)
#endif


ssize_t read_code ( char **code, FILE *in );
int run_in_parallel ( char *code, struct cmdargs *args, int argc, char *argv[], int optind ) ;

/** @brief  
  * 
  * @param args contains the parsed cmd-line options & arguments.
  * @param argc number of cmd-line arguments.
  * @param argv list of cmd-line arguments
  * @param optind index of the first non-option cmd-line argument.
  * 
  * @return exit status for main() to return.
  */
int foreach_parallel ( struct cmdargs *args, int argc, char *argv[], int optind ){
	FILE *in;
	char *code;

	int ret;

	if ( optind == argc ) {
		fprintf(stderr, "%s: no input values provided.\n",
				getenv("_"));
		return EXIT_HELP;
	}

	if ( args->file ) {
		in = fopen(args->file, "r");
		if ( ! in ) {
			warn(args->file);
			return EXIT_FILE_ERR;
		}
	}
	else {
		in = stdin;
	}

	ret = read_code( &code, in );
	if ( ret < 0 ) {
		fprintf(stderr, "%s: out of memory\n", getenv("_"));
		return EXIT_MEM_ERR;
	}
	else if ( ret == 0 ) {
		fprintf(stderr, "%s: nothing to execute.\n", getenv("_"));
		return EXIT_HELP;
	}

	run_in_parallel(code, args, argc, argv, optind);

	free(code);
	return EXIT_OKAY;
}

ssize_t read_code ( char **code, FILE *in ) {
	size_t code_len, code_sz, n_read;

	*code = malloc(1024);
	code_sz = 1024;
	code_len = 0;
	while ( ! feof(in) ) {
		n_read = fread(*code + code_len, 1, code_sz - code_len, in);
		code_len += n_read;
		if ( code_len == code_sz ) {
			*code = realloc(*code, code_sz + 1024);
			if ( ! code )
				return -1;
		}
	}
	(*code)[code_len] = '\0';
	return code_len;
}

int wait_for_child () {
	int status;
	for ( ;; ) {
		if ( wait(&status) < 0 ) {
			if ( errno == EINTR ) {
				continue;
			}
			else {
				warn("wait error");
				return -1;
			}
		}
		break;
	}
	return status;
}

int run_in_parallel ( char *code, struct cmdargs *args, int argc, char *argv[], int optind ) {
	int max_procs = DEFAULT_MAX_PROCS;
	int active_children = 0;
	pid_t p;
	int i, ret = 0;

	if ( args->max_procs ) {
		if ( sscanf(args->max_procs, "%d", &max_procs) != 1
		  || max_procs == 0 ) {
			fprintf(stderr, "%s: invalid value for --limit: %s - using default.\n",
					getenv("_"), args->max_procs);
			max_procs = DEFAULT_MAX_PROCS;
		}
	}

	for ( i = optind; i < argc; i++ ) {
		p = fork();
		if ( p < 0 ) {
			warn("fork error");
			return -1;
		}
		else if ( p == 0 ) {
			/* in the child proc */
			setenv(args->variable, argv[i], 1);
			execl(getenv("SHELL"), getenv("SHELL"), "-c", code, NULL);
		}
		else {
			/* in the parent proc */
			active_children++;
			if ( active_children >= max_procs ) {
				DEBUG_PRINT("limit reached: waiting for a child to exit.");
				if ( (ret = wait_for_child()) != 0 )
					break;
				active_children--;
			}
		}
	}

	if ( ret == 0 ) {
		while ( active_children > 0 ) {
			DEBUG_PRINT("cleanup: waiting for a child to exit.");
			if ( (ret = wait_for_child()) != 0 )
				break;
			active_children--;
		}
	}

	return ret;
}
