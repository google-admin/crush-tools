#include <stdio.h>
#include <stdlib.h>
#include <qsort_helper.h>


void test_qsort_strcmp();
void test_qsort_intcmp();
void test_qsort_uintcmp();

int main ( int argc, char *argv[] ) {
	test_qsort_strcmp();
	test_qsort_intcmp();
	test_qsort_uintcmp();
	return 0;
}

void test_qsort_strcmp () {
	char *array[] = {"hello", "world", "there"};
	qsort(array, 3, sizeof(char *), (qsort_cmp_func_t) qsort_strcmp);

	if ( strcmp(array[0], "hello") == 0
	  && strcmp(array[1], "there") == 0
	  && strcmp(array[2], "world") == 0
	)
		printf("qsort_strcmp:\tok.\n");
	else
		printf("qsort_strcmp:\tfailed: (%s, %s, %s)\n",
				array[0], array[1], array[2]);
}

void test_qsort_intcmp () {
	int array[] = { 3,5,-1,2,1 };
	qsort(array, 5, sizeof(int), (qsort_cmp_func_t) qsort_intcmp);
	if ( array[0] == -1
	  && array[1] == 1
	  && array[2] == 2
	  && array[3] == 3
	  && array[4] == 5
	)
		printf("qsort_intcmp:\tok\n");
	else
		printf("qsort_intcmp:\tfailed: (%d, %d, %d, %d, %d)\n",
				array[0], array[1], array[2], array[3],
				array[4] );
}

void test_qsort_uintcmp () {
	unsigned int array[] = { 3,5,1,2,1 };
	qsort(array, 5, sizeof(int), (qsort_cmp_func_t) qsort_intcmp);
	if ( array[0] == 1
	  && array[1] == 1
	  && array[2] == 2
	  && array[3] == 3
	  && array[4] == 5
	)
		printf("qsort_uintcmp:\tok\n");
	else
		printf("qsort_uintcmp:\tfailed: (%d, %d, %d, %d, %d)\n",
				array[0], array[1], array[2], array[3],
				array[4] );
}



