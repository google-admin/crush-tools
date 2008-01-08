#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_FFUTILS_H
# include <ffutils.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>	/* open64() */
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>	/* off64_t & close() */
#endif

#ifdef HAVE_ERR_H
# include <err.h>	/* warn() */
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

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
int my_peek_keys ( char *peek_line, char *current_line ) ;

#endif /* MERGEKEYS_H */
