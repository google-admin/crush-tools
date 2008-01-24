#if HAVE_CONFIG_H
# include <config.h>
#endif

#if HAVE_FFUTILS_H
# include <ffutils.h>
#endif

#if HAVE_FCNTL_H
# include <fcntl.h>	/* open64() */
#endif

#if HAVE_UNISTD_H
# include <unistd.h>	/* off64_t & close() */
#endif

#if HAVE_ERR_H
# include <err.h>	/* warn() */
#endif

#if HAVE_LOCALE_H
# include <locale.h>
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif

#ifndef DELTAFORCE_H
#define DELTAFORCE_H

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


int merge_files( FILE *a, FILE *b, FILE *out, struct cmdargs *args ) ;
int compare_keys ( char *buffer_left, char *buffer_right );

#endif /* DELTAFORCE_H */
