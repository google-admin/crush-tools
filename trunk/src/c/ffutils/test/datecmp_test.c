#include <stdio.h>

#include "ffutils.h"

int main ( int argc, char *argv[] ) {

	char *date_a, *date_b;
	char line[64];

	int cmp;

	while ( fgets(line, 64, stdin) ) {

		line[19] = '\0';
		line[39] = '\0';

		date_a = line;
		date_b = &(line[20]);

		cmp = mdyhms_datecmp(date_a, date_b);
		/*
		printf("%s %s %s\n",
			date_a,
			cmp == 0 ? "=" : (cmp > 0 ? ">" : "<"),
			date_b );
		*/
	}

	return 0;
}
