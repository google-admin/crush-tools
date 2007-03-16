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
void test_field_str(void);

int main ( int argc, char *argv[] ) {
	test_fields_in_line();
	test_get_line_field();
	test_field_start();
	test_mdyhms_datecmp();
	test_chomp();
	test_nextfile();
	test_expand_chars();
	test_expand_nums();
	test_cut_field();
	test_field_str();

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

void test_cut_field ( void ) {
	int n_errors = 0;
	char result[32];
	char *new_field;

	char *TL0 = "a;b;c;d;e";
	char *TE0 = "b;c;d;e";
	char *TE1 = "a;b;c;d";
	char *TE2 = "a;b;d;e";

	strncpy(result, TL0, 31);
	new_field = cut_field(result, 0, ";");
	if ( ! str_eq( new_field, TE0 ) ) {
		fprintf(stderr,
			"failure: cut_field( \"%s\", 0, \";\" ) returned %s instead of %s\n",
			TL0, new_field, TE0);
		n_errors++;
	}
	free(new_field);
        new_field = cut_field(result, 4, ";");
        if ( ! str_eq( new_field, TE1 ) ) {
                fprintf(stderr,
                        "failure: cut_field( \"%s\", 4, \";\" ) returned %s instead of %s\n",
                        TL0, new_field, TE1);
                n_errors++;
        }
	free(new_field);
        new_field = cut_field(result, 2, ";");
        if ( ! str_eq( new_field, TE2 ) ) {
                fprintf(stderr,
                        "failure: cut_field( \"%s\", 2, \";\" ) returned %s instead of %s\n",
                        TL0, new_field, TE2);
                n_errors++;
        }
	free(new_field);

	if ( n_errors == 0 ) {
		printf("cut_field(): ok\n");
	}

}

void test_field_str () {
	int n_errors = 0;
	int n;

	/* test case: empty line - considered a no-match */
	char  *TL0 = "";
	char  *TV0 = "hello";
	char  *TD0 = "|";
	int    TE0 = -1;

	/* test case: null search value - considered an error */
	char  *TL1 = "hello|world";
	char  *TV1 = NULL;
	char  *TD1 = "|";
	int    TE1 = -2;

	/* test case: null delimiter, no-match */
	char  *TL2 = "hello world";
	char  *TV2 = "hello";
	char  *TD2 = NULL;
	int    TE2 = -1;

	/* test case: null delimiter, match */
	char  *TL3 = "hello";
	char  *TV3 = "hello";
	char  *TD3 = NULL;
	int    TE3 = 0;

	/* test case (x 3): field exists, single-char delimiter*/
	char  *TL4 = "hello there world";
	char  *TV4 = "hello";
	char  *TD4 = " ";
	int    TE4 = 0;

	char  *TL5 = "hello there world";
	char  *TV5 = "there";
	char  *TD5 = " ";
	int    TE5 = 1;

	char  *TL6 = "hello there world";
	char  *TV6 = "world";
	char  *TD6 = " ";
	int    TE6 = 2;

	/* test case: field exists, multi-char delimiter */
	char  *TL7 = "hello||dev||prac";
	char  *TV7 = "prac";
	char  *TD7 = "||";
	int    TE7 = 2;

	/* test case: field exists, multi-char delimiter, empty value */
	char  *TL8 = "hello||dev||||prac";
	char  *TV8 = "";
	char  *TD8 = "||";
	int    TE8 = 2;

	/* test case: field doesn't exist */
	char  *TL9 = "hello there world";
	char  *TV9 = "weee";
	char  *TD9 = " ";
	int    TE9 = -1;

	/* test case: field exists, multi-char delimiter */
	n = field_str( TV0, TL0, TD0 );
	if ( n != TE0 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV0, TL0, TD0, n, TE0 );
		n_errors++;
	}

	n = field_str( TV1, TL1, TD1 );
	if ( n != TE1 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV1, TL1, TD1, n, TE1 );
		n_errors++;
	}

	n = field_str( TV2, TL2, TD2 );
	if ( n != TE2 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV2, TL2, TD2, n, TE2 );
		n_errors++;
	}

	n = field_str( TV3, TL3, TD3 );
	if ( n != TE3 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV3, TL3, TD3, n, TE3 );
		n_errors++;
	}

	n = field_str( TV4, TL4, TD4 );
	if ( n != TE4 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV4, TL4, TD4, n, TE4 );
		n_errors++;
	}

	n = field_str( TV5, TL5, TD5 );
	if ( n != TE5 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV5, TL5, TD5, n, TE5 );
		n_errors++;
	}

	n = field_str( TV6, TL6, TD6 );
	if ( n != TE6 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV6, TL6, TD6, n, TE6 );
		n_errors++;
	}

	n = field_str( TV7, TL7, TD7 );
	if ( n != TE7 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV7, TL7, TD7, n, TE7 );
		n_errors++;
	}

	n = field_str( TV8, TL8, TD8 );
	if ( n != TE8 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV8, TL8, TD8, n, TE8 );
		n_errors++;
	}

	n = field_str( TV9, TL9, TD9 );
	if ( n != TE9 ) {
		fprintf(stderr,
			"failure: field_str( \"%s\", \"%s\", \"%s\" ) returned %d instead of %d\n",
			TV9, TL9, TD9, n, TE9 );
		n_errors++;
	}

	if ( n_errors == 0 ) {
		printf("field_str(): ok\n");
	}
}
