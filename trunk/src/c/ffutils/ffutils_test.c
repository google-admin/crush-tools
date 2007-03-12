#include "ffutils.h"

void test_fields_in_line(void);
void test_get_line_field(void);
void test_field_start(void);
void test_mdyhms_datecmp(void);
void test_chomp(void);
void test_nextfile(void);
void test_expand_chars(void);
void test_expand_nums(void);
void test_cut_field(void);

int main ( int argc, char *argv[] ) {
	test_fields_in_line();
	test_get_line_field();
	test_field_start();
	test_mdyhms_datecmp();
	test_chomp();
	test_nextfile();
	test_expand_chars();
	test_expand_nums();

	return 0;
}

void test_fields_in_line () {
	int n_errors = 0;
	size_t n;

	char  *TL0 = "";
	char  *TD0 = "|";
	size_t TE0 = 1;

	char  *TL1 = " hello world";
	char  *TD1  = " ";
	size_t TE1 = 3;

	char  *TL2 = "hello||dev||prac||";
	char  *TD2 = "||";
	size_t TE2 = 4;

	char  *TL3 = NULL;
	char  *TD3 = ",";
	size_t TE3 = 0;

	n = fields_in_line( TL0, TD0 );
	if ( n != TE0 ) {
		fprintf(stderr,
			"failure: fields_in_line( \"%s\", \"%s\" ) returned %u instead of %u\n",
			TL0, TD0, n, TE0 );
		n_errors++;
	}

	n = fields_in_line( TL1, TD1 );
	if ( n != TE1 ) {
		fprintf(stderr,
			"failure: fields_in_line( \"%s\", \"%s\" ) returned %u instead of %u\n",
			TL1, TD1, n, TE1 );
		n_errors++;
	}

	n = fields_in_line( TL2, TD2 );
	if ( n != TE2 ) {
		fprintf(stderr,
			"failure: fields_in_line( \"%s\", \"%s\" ) returned %u instead of %u\n",
			TL2, TD2, n, TE2 );
		n_errors++;
	}

	n = fields_in_line( TL3, TD3 );
	if ( n != TE3 ) {
		fprintf(stderr,
			"failure: fields_in_line( \"%s\", \"%s\" ) returned %u instead of %u\n",
			TL3, TD3, n, TE3 );
		n_errors++;
	}

	if ( n_errors == 0 ) {
		printf("fields_in_line(): ok\n");
	}
}

void test_get_line_field ( void ) {
	int n_errors = 0;
	printf("get_line_field(): test not implemented\n");
}

void test_field_start ( void ) {
	int n_errors = 0;
	printf("field_start(): test not implemented\n");
}

void test_mdyhms_datecmp ( void ) {
	int n_errors = 0;
	printf("mdyhms_datecmp(): test not implemented\n");
}

void test_chomp ( void ) {
	int n_errors = 0;
	printf("chomp(): test not implemented\n");
}

void test_nextfile ( void ) {
	int n_errors = 0;
	printf("nextfile(): test not implemented\n");
}

void test_expand_chars ( void ) {
	int n_errors = 0;

	char *TL0  = "\t";
	char TE0[] = {0x09, 0x00};

	char *TL1 = "no special chars";
	char *TE1 = "no special chars";

	char TL2[] = {0x5c, 0x5c, 0x00}; /* \\ */
	char TE2[] = {0x5c, 0x00};

	expand_chars( TL0 );
	if ( strcmp( TL0, TE0 ) != 0 ) {
		fprintf(stderr,
			"failure: expand_chars( \"%s\" ) resulted in  \"%s\" instead of \"%s\"\n",
			TL0, TE0);
		n_errors++;
	}

	expand_chars( TL1 );
	if ( strcmp( TL1, TE1 ) != 0 ) {
		fprintf(stderr,
			"failure: expand_chars( \"%s\" ) resulted in  \"%s\" instead of \"%s\"\n",
			TL1, TE1);
		n_errors++;
	}

	expand_chars( TL2 );
	if ( strcmp( TL2, TE2 ) != 0 ) {
		fprintf(stderr,
			"failure: expand_chars( \"%s\" ) resulted in  \"%s\" instead of \"%s\"\n",
			TL2, TE2);
		n_errors++;
	}

	if ( n_errors == 0 ) {
		printf("expand_chars(): ok\n");
	}
}

void test_expand_nums ( void ) {
	int n_errors = 0;

	ssize_t n;

	int *target = NULL;
	size_t target_size = 0;
	char *tmpstr;

	char   *TL0 = "1,2,3";
	ssize_t TE0 = 3;

	char   *TL1 = "1,3-5";
	ssize_t TE1 = 4;

	char   *TL2 = "1,three";
	ssize_t TE2 = -2;

	char   *TL3 = NULL;
	ssize_t TE3 = 0;


	tmpstr = malloc(16);

	strcpy(tmpstr, TL0);
	n = expand_nums( tmpstr, &target, &target_size );
	if ( n != TE0 ) {
		fprintf(stderr,
			"failure: expand_nums( \"%s\", &target, &target_size ) returned %u instead of %u\n",
			n, TL0);
		n_errors++;
	}

	strcpy(tmpstr, TL1);
	n = expand_nums( tmpstr, &target, &target_size );
	if ( n != TE1 ) {
		fprintf(stderr,
			"failure: expand_nums( \"%s\", &target, &target_size ) returned %u instead of %u\n",
			n, TL1);
		n_errors++;
	}

	strcpy(tmpstr, TL2);
	n = expand_nums( tmpstr, &target, &target_size );
	if ( n != TE2 ) {
		fprintf(stderr,
			"failure: expand_nums( \"%s\", &target, &target_size ) returned %u instead of %u\n",
			n, TL2);
		n_errors++;
	}

	/* TL3 is null */
	n = expand_nums( TL3, &target, &target_size );
	if ( n != TE3 ) {
		fprintf(stderr,
			"failure: expand_nums( \"%s\", &target, &target_size ) returned %u instead of %u\n",
			n, TL3);
		n_errors++;
	}

	if ( n_errors == 0 ) {
		printf("expand_nums(): ok\n");
	}

	free(target);
	free(tmpstr);
}

/*
void test_cut_field ( void ) {
	int n_errors = 0;
	char result[32];

	char *TL0 = "a;b;c;d;e";
	char *TE0 = "b;c;d;e";

	strncpy(result, TL0, 31);
	cut_field(result, 0, ";");
	if ( ! str_eq( result, TE0 ) ) {
		fprintf(stderr,
			"failure: cut_field( \"%s\", 0, \";\" ) returned %s instead of %s\n",
			TL0, result, TE0);
		n_errors++;
	}

	if ( n_errors == 0 ) {
		printf("expand_nums(): ok\n");
	}

}
*/

