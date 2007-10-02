#include <ffutils.h>
#include <fcntl.h>	/* open64() */
#include <unistd.h>	/* off64_t & close() */
#include <err.h>	/* warn() */
#include <locale.h>

#ifndef MERGEKEYS_H
#define MERGEKEYS_H

#define MAX_FIELD_LEN 255


/* macros to clarify the semantics of key comparisons. */

/* these are used to compare "keycmp" */
#define LEFT_LT_RIGHT(n) (n) < 0
#define LEFT_LE_RIGHT(n) (n) <= 0
#define LEFT_EQ_RIGHT(n) (n) == 0
#define LEFT_GT_RIGHT(n) (n) > 0
#define LEFT_GE_RIGHT(n) (n) >= 0

/* these are used to assign to keycmp */
#define LEFT_GREATER  1
#define RIGHT_GREATER -1
#define LEFT_RIGHT_EQUAL   0


enum join_type_t {
	join_type_outer,
	join_type_inner,
	join_type_left_outer,
	join_type_right_outer,
};

int merge_files( FILE *a, FILE *b, enum join_type_t join_type, FILE *out, struct cmdargs *args ) ;

void classify_fields ( char   *left_header, char *right_header );
int compare_keys ( char *buffer_left, char *buffer_right );
void join_lines ( char *left_line, char *right_line, FILE *out );
int peek_keys ( FILE *file, char *current_line ) ;

#endif /* MERGEKEYS_H */
