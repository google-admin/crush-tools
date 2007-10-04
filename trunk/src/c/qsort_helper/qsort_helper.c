#include <qsort_helper.h>

int qsort_strcmp ( const char **a, const char **b ) {
	return strcmp(*a, *b);
}

int qsort_intcmp ( const int *a, const int *b ) {
	return *a - *b;
}

int qsort_uintcmp ( const unsigned int *a, const unsigned int *b ) {
	return *a - *b;
}

