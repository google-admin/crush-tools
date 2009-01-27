/*****************************************
   Copyright 2008 Google Inc.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************/


#include <stdio.h>
#include <linklist.h>

#include <ffutils.h>

int XFAIL(int n) {
  if (n == 0)
    return 1;
  return 0;
}

/* for output padding */
#define FUNC_NAME_FMT "%16s"

int test_fields_in_line(void);
int test_get_line_field(void);
int test_get_line_pos(void);
int test_field_start(void);
int test_mdyhms_datecmp(void);
int test_chomp(void);
int test_nextfile(void);
int test_expand_chars(void);
int test_expand_nums(void);
int test_expand_label_list(void);
int test_cut_field(void);
int test_field_str(void);

/* void test_get_spot_tag_attributes(void); */

int main(int argc, char *argv[]) {
  int errs = 0;
  printf("\n-------------\n");
  errs += test_fields_in_line();
  errs += test_get_line_field();
  errs += test_get_line_pos();
  errs += test_field_start();
  errs += test_mdyhms_datecmp();
  errs += test_chomp();
  errs += test_nextfile();
  errs += test_expand_chars();
  errs += test_expand_nums();
  errs += test_expand_label_list();
  errs += test_cut_field();
  errs += test_field_str();
  /* errs += test_get_spot_tag_attributes(); */
  printf("-------------\n");
  if (errs)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}

int test_fields_in_line() {
  int n_errors = 0;
  size_t n;

  char *TL0 = "";
  char *TD0 = "|";
  size_t TE0 = 1;

  char *TL1 = " hello world";
  char *TD1 = " ";
  size_t TE1 = 3;

  char *TL2 = "hello||dev||prac||";
  char *TD2 = "||";
  size_t TE2 = 4;

  char *TL3 = NULL;
  char *TD3 = ",";
  size_t TE3 = 0;

  n = fields_in_line(TL0, TD0);
  if (n != TE0) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 0)\n\treturned %u instead of %u\n",
            "fields_in_line()", n, TE0);
    n_errors++;
  }

  n = fields_in_line(TL1, TD1);
  if (n != TE1) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 1)\n\treturned %u instead of %u\n",
            "fields_in_line()", n, TE1);
    n_errors++;
  }

  n = fields_in_line(TL2, TD2);
  if (n != TE2) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 2)\n\treturned %u instead of %u\n",
            "fields_in_line()", n, TE2);
    n_errors++;
  }

  n = fields_in_line(TL3, TD3);
  if (n != TE3) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 3)\n\treturned %u instead of %u\n",
            "fields_in_line()", n, TE3);
    n_errors++;
  }

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "fields_in_line()");
  }
  return n_errors;
}

int test_get_line_field(void) {
  int n_errors = 0;

  char buffer[6];
  size_t bufsz = 6;
  int n;

  char *TL0 = "this,is,a,test\n";
  char *TL1 = "this,is,a,test";
  char *TD = ",";

  int TF0 = 0;
  char *TE0_buffer = "this";
  int TE0_ret = 4;

  int TF1 = 1;
  char *TE1_buffer = "is";
  int TE1_ret = 2;

  int TF2 = 3;
  char *TE2_buffer = "test";
  int TE2_ret = 4;

  int TF3 = 4;
  char *TE3_buffer = "";
  int TE3_ret = -1;

#define RUN_TEST(testno, lnvar) \
	n = get_line_field( buffer, (lnvar), bufsz, TF##testno, TD ); \
	if ( n != TE##testno##_ret \
	  || strcmp(buffer, TE##testno##_buffer) != 0 ) { \
		fprintf(stderr, \
			FUNC_NAME_FMT \
			": failed (test " #testno ")\n\treturned (%d, \"%s\") instead of (%d, \"%s\")\n", \
			"get_line_field()", \
			n, buffer, TE##testno##_ret, TE##testno##_buffer ); \
		n_errors++; \
	}

  RUN_TEST(0, TL0)
    RUN_TEST(0, TL1)
    RUN_TEST(1, TL0)
    RUN_TEST(2, TL0)
    RUN_TEST(3, TL0)
#undef RUN_TEST
    return n_errors;
}

int test_get_line_pos(void) {
  int n_errors = 0;

  /* populated last field */
  char *TD0 = ",";
  char *TL0 = "hello\n";
  int TF0 = 0;
  int TE0_ret = 5;
  int TE0_start = 0;
  int TE0_end = 4;

  /* empty last field */
  char *TD1 = ",";
  char *TL1 = "hello,,\n";
  int TF1 = 2;
  int TE1_ret = 0;
  int TE1_start = 7;
  int TE1_end = 7;

  /* empty middle field */
  char *TD2 = ",";
  char *TL2 = "hello,,\n";
  int TF2 = 1;
  int TE2_ret = 0;
  int TE2_start = 6;
  int TE2_end = 6;

  /* single-char in last field */
  char *TD3 = ",";
  char *TL3 = "hello,i,i\n";
  int TF3 = 2;
  int TE3_ret = 1;
  int TE3_start = 8;
  int TE3_end = 8;

  /* single-char in middle field */
  char *TD4 = ",";
  char *TL4 = "hello,i,i\n";
  int TF4 = 1;
  int TE4_ret = 1;
  int TE4_start = 6;
  int TE4_end = 6;

  int n, start, end;

#define RUN_TEST(testno) \
	n = get_line_pos( TL##testno, TF##testno, TD##testno, &start, &end ); \
	if ( n != TE##testno##_ret \
	  || start != TE##testno##_start \
	  || end != TE##testno##_end ) { \
		fprintf(stderr, \
			FUNC_NAME_FMT \
			": failed (test " #testno ")\n\treturned (%d, %d, %d) instead of (%d, %d, %d)\n", \
			"get_line_pos()", \
			n, start, end, TE##testno##_ret, TE##testno##_start, TE##testno##_end ); \
		n_errors++; \
	}

  RUN_TEST(0);
  RUN_TEST(1);
  RUN_TEST(2);
  RUN_TEST(3);
  RUN_TEST(4);

#undef RUN_TEST

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "get_line_pos()");
  }

  return n_errors;
}

int test_field_start(void) {
  int n_errors = 0;

  char *TL = "this,is,a,test\n";
  char *TD = ",";
  char *ret;

  int TF0 = 1;
  char *TE0 = TL;

  int TF1 = 2;
  char *TE1 = TL + 5;

  int TF2 = 4;
  char *TE2 = TL + 10;

  int TF3 = 5;
  char *TE3 = NULL;

#define RUN_TEST(testno) \
	ret = field_start( TL, TF##testno, TD ); \
	if ( ret != TE##testno ) { \
		fprintf(stderr, \
			FUNC_NAME_FMT \
			": failed (test " #testno ")\n\treturned %s instead of %s\n", \
			"field_start()", \
			ret, TE##testno ); \
		n_errors++; \
	}

  RUN_TEST(0)
    RUN_TEST(1)
    RUN_TEST(2)
    RUN_TEST(3)
#undef RUN_TEST
    if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "field_start()");
  }

  return n_errors;
}

int test_mdyhms_datecmp(void) {
  int n_errors = 0;
  int ret;

  char *TD0_a = "09-06-2008 11:59:59";  /* input date a */
  char *TD0_b = "10-04-2008 11:59:59";  /* input date b */
  int TE0 = -1;                 /* expected return */

  char *TD1_a = "09-06-2008 11:59:59";
  char *TD1_b = "09-06-2008 11:59:59";
  int TE1 = 0;

  char *TD2_a = "10-04-2008 11:59:59";
  char *TD2_b = "09-06-2008 11:59:59";
  int TE2 = 1;

#define RUN_TEST(testno) \
	ret = mdyhms_datecmp( TD##testno##_a, TD##testno##_b ); \
	if ( ret != TE##testno ) { \
		fprintf(stderr, \
			FUNC_NAME_FMT \
			": failed (test " #testno ")\n\treturned %d instead of %d\n", \
			"mdyhms_datecmp()", \
			ret, TE##testno ); \
		n_errors++; \
	}

  RUN_TEST(0)
    RUN_TEST(1)
    RUN_TEST(2)
#undef RUN_TEST
    if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "mdyhms_datecmp()");
  }

  return n_errors;
}

int test_chomp(void) {
  int n_errors = 0;

  char T0_pre[] = "hello world\n";
  char T0_post[] = "hello world";

  char T1_pre[] = "hello world\r\n";
  char T1_post[] = "hello world";

  char T2_pre[] = "hello world";
  char T2_post[] = "hello world";

#define RUN_TEST(testno) \
	chomp( T##testno##_pre ); \
	if ( strcmp( T##testno##_pre, T##testno##_post ) != 0 ) { \
		fprintf(stderr, \
			FUNC_NAME_FMT \
			": failed (test " #testno ")\n\treturned \"%s\" instead of \"%s\"\n", \
			"chomp()", \
			T##testno##_pre, T##testno##_post ); \
		n_errors++; \
	}

  RUN_TEST(0)
    RUN_TEST(1)
    RUN_TEST(2)
#undef RUN_TEST
    if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "chomp()");
  }

  return n_errors;
}

int test_nextfile(void) {
  int n_errors = 0;
  printf(FUNC_NAME_FMT ": skip (not implemented)\n", "nextfile()");
  return n_errors;
}

int test_expand_chars(void) {
  int n_errors = 0;

  char *TL0 = "\t";
  char TE0[] = { 0x09, 0x00 };

  char *TL1 = "no special chars";
  char *TE1 = "no special chars";

  char TL2[] = { 0x5c, 0x5c, 0x00 };  /* \\ */
  char TE2[] = { 0x5c, 0x00 };

  expand_chars(TL0);
  if (strcmp(TL0, TE0) != 0) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed\n\treturned \"%s\" instead of \"%s\"\n",
            "expand_chars()", TL0, TE0);
    n_errors++;
  }

  expand_chars(TL1);
  if (strcmp(TL1, TE1) != 0) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed\n\treturned \"%s\" instead of \"%s\"\n",
            "expand_chars()", TL1, TE1);
    n_errors++;
  }

  expand_chars(TL2);
  if (strcmp(TL2, TE2) != 0) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed\n\treturned \"%s\" instead of \"%s\"\n",
            "expand_chars()", TL2, TE2);
    n_errors++;
  }

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "expand_chars()");
  }
  return n_errors;
}

int test_expand_nums(void) {
  int n_errors = 0;

  ssize_t n;

  int *target = NULL;
  size_t target_size = 0;
  char *tmpstr;

  char *TL0 = "1,2,3";
  ssize_t TE0 = 3;

  char *TL1 = "1,3-5";
  ssize_t TE1 = 4;

  char *TL2 = "1,three";
  ssize_t TE2 = -2;

  char *TL3 = NULL;
  ssize_t TE3 = 0;


  tmpstr = malloc(16);

  strcpy(tmpstr, TL0);
  n = expand_nums(tmpstr, &target, &target_size);
  if (n != TE0) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 0)\n\treturned %u instead of %u\n",
            "expand_nums()", n, TE0);
    n_errors++;
  }

  strcpy(tmpstr, TL1);
  n = expand_nums(tmpstr, &target, &target_size);
  if (n != TE1) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 1)\n\treturned %u instead of %u\n",
            "expand_nums()", n, TE1);
    n_errors++;
  }

  strcpy(tmpstr, TL2);
  n = expand_nums(tmpstr, &target, &target_size);
  if (n != TE2) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 2)\n\treturned %u instead of %u\n",
            "expand_nums()", n, TE2);
    n_errors++;
  }

  /* TL3 is null */
  n = expand_nums(TL3, &target, &target_size);
  if (n != TE3) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 3)\n\treturned %u instead of %u\n",
            "expand_nums()", n, TE3);
    n_errors++;
  }

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "expand_nums()");
  }

  free(target);
  free(tmpstr);

  return n_errors;
}


int test_expand_label_list(void) {
	int n_errors = 0;
	char *line = "L1|L2|L3|L4|L5|L,6\n";
  char *delim = "|";
  int *array = NULL;
  size_t array_sz = 0;
  int retval, i;

	char *T0_labels = "L1,L3";
	int T0_retval = 2;
	int T0_elems[] = {1, 3};

	char *T1_labels = "L1,L7";
	int T1_retval = -1;

  char *T2_labels = "L\\,6";
  int T2_retval = 1;
  int T2_elems[] = {6};

  retval = expand_label_list(T0_labels, line, delim, &array, &array_sz);
  if (retval != T0_retval) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 0)\n\treturned %u instead of %u\n",
            "expand_label_list()", retval, T0_retval);
  	n_errors++;
  } else {
    for (i = 0; i < retval; i++) {
    	if (T0_elems[i] != array[i]) {
        fprintf(stderr,
                FUNC_NAME_FMT
                ": failed (test 0)\n\telem %d is %d instead of %d\n",
                "expand_label_list()", i, array[i], T0_elems[i]);
      }
    }
  }

  retval = expand_label_list(T1_labels, line, delim, &array, &array_sz);
  if (retval != T1_retval) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 1)\n\treturned %u instead of %u\n",
            "expand_label_list()", retval, T0_retval);
  	n_errors++;
  }

  retval = expand_label_list(T2_labels, line, delim, &array, &array_sz);
  if (retval != T2_retval) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 2)\n\treturned %u instead of %u\n",
            "expand_label_list()", retval, T2_retval);
  	n_errors++;
  } else {
    for (i = 0; i < retval; i++) {
    	if (T2_elems[i] != array[i]) {
        fprintf(stderr,
                FUNC_NAME_FMT
                ": failed (test 2)\n\telem %d is %d instead of %d\n",
                "expand_label_list()", i, array[i], T2_elems[i]);
      }
    }
  }

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "expand_label_list()");
  }

  free(array);
  return n_errors;
}


int test_cut_field(void) {
  int n_errors = 0;
  char result[32];
  char *new_field;

  char *TL0 = "a;b;c;d;e";
  char *TE0 = "b;c;d;e";
  char *TE1 = "a;b;c;d";
  char *TE2 = "a;b;d;e";

  strncpy(result, TL0, 31);
  new_field = cut_field(result, 0, ";");
  if (!str_eq(new_field, TE0)) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 0)\n\treturned %s instead of %s\n",
            "cut_field()", new_field, TE0);
    n_errors++;
  }
  free(new_field);
  new_field = cut_field(result, 4, ";");
  if (!str_eq(new_field, TE1)) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 1)\n\treturned %s instead of %s\n",
            "cut_field()", new_field, TE1);
    n_errors++;
  }
  free(new_field);
  new_field = cut_field(result, 2, ";");
  if (!str_eq(new_field, TE2)) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 2)\n\treturned %s instead of %s\n",
            "cut_field()", new_field, TE2);
    n_errors++;
  }
  free(new_field);

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "cut_field()");
  }

  return n_errors;
}

int test_field_str() {
  int n_errors = 0;
  int n;

  /* test case: empty line - considered a no-match */
  char *TL0 = "";
  char *TV0 = "hello";
  char *TD0 = "|";
  int TE0 = -1;

  /* test case: null search value - considered an error */
  char *TL1 = "hello|world";
  char *TV1 = NULL;
  char *TD1 = "|";
  int TE1 = -2;

  /* test case: null delimiter, no-match */
  char *TL2 = "hello world";
  char *TV2 = "hello";
  char *TD2 = NULL;
  int TE2 = -1;

  /* test case: null delimiter, match */
  char *TL3 = "hello";
  char *TV3 = "hello";
  char *TD3 = NULL;
  int TE3 = 0;

  /* test case (x 3): field exists, single-char delimiter */
  char *TL4 = "hello there world";
  char *TV4 = "hello";
  char *TD4 = " ";
  int TE4 = 0;

  char *TL5 = "hello there world";
  char *TV5 = "there";
  char *TD5 = " ";
  int TE5 = 1;

  char *TL6 = "hello there world";
  char *TV6 = "world";
  char *TD6 = " ";
  int TE6 = 2;

  /* test case: field exists, multi-char delimiter */
  char *TL7 = "hello||dev||prac";
  char *TV7 = "prac";
  char *TD7 = "||";
  int TE7 = 2;

  /* test case: field exists, multi-char delimiter, empty value */
  char *TL8 = "hello||dev||||prac";
  char *TV8 = "";
  char *TD8 = "||";
  int TE8 = 2;

  /* test case: field doesn't exist */
  char *TL9 = "hello there world";
  char *TV9 = "weee";
  char *TD9 = " ";
  int TE9 = -1;

  /* test case: field exists, multi-char delimiter */
  n = field_str(TV0, TL0, TD0);
  if (n != TE0) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 0)\n\treturned %d instead of %d\n",
            "field_str()", n, TE0);
    n_errors++;
  }

  n = field_str(TV1, TL1, TD1);
  if (n != TE1) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 1)\n\treturned %d instead of %d\n",
            "field_str()", n, TE1);
    n_errors++;
  }

  n = field_str(TV2, TL2, TD2);
  if (n != TE2) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 2)\n\treturned %d instead of %d\n",
            "field_str()", n, TE2);
    n_errors++;
  }

  n = field_str(TV3, TL3, TD3);
  if (n != TE3) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 3)\n\treturned %d instead of %d\n",
            "field_str()", n, TE3);
    n_errors++;
  }

  n = field_str(TV4, TL4, TD4);
  if (n != TE4) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 4)\n\treturned %d instead of %d\n",
            "field_str()", n, TE4);
    n_errors++;
  }

  n = field_str(TV5, TL5, TD5);
  if (n != TE5) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 5)\n\treturned %d instead of %d\n",
            "field_str()", n, TE5);
    n_errors++;
  }

  n = field_str(TV6, TL6, TD6);
  if (n != TE6) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 6)\n\treturned %d instead of %d\n",
            "field_str()", n, TE6);
    n_errors++;
  }

  n = field_str(TV7, TL7, TD7);
  if (n != TE7) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 7)\n\treturned %d instead of %d\n",
            "field_str()", n, TE7);
    n_errors++;
  }

  n = field_str(TV8, TL8, TD8);
  if (n != TE8) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 8)\n\treturned %d instead of %d\n",
            "field_str()", n, TE8);
    n_errors++;
  }

  n = field_str(TV9, TL9, TD9);
  if (n != TE9) {
    fprintf(stderr,
            FUNC_NAME_FMT
            ": failed (test 9)\n\treturned %d instead of %d\n",
            "field_str()", n, TE9);
    n_errors++;
  }

  if (n_errors == 0) {
    printf(FUNC_NAME_FMT ": passed\n", "field_str()");
  }
  return n_errors;
}

/*
int test_get_spot_tag_attributes ( void ) {
	int spot_id = 673394;
	char *db_name = "PROD";
	char *db_uid  = getenv("DB_UID");
	char *db_pass = getenv("DB_PWD");
	llist_t tag_list;

	int n_tags;

	ll_list_init( &tag_list, free, NULL );

	n_tags = get_spot_tag_attributes(&tag_list, spot_id, db_name, db_uid, db_pass);

	if ( n_tags == 0 ) {
		printf("get_spot_tag_attributes(): failure (no tags returned)\n");
	}
	else {
		llist_node_t *curnode;
		struct spotlight_tag_attributes *tag_attribs;

		printf("get_spot_tag_attributes(): ok\n");

		for ( curnode = tag_list.head;
		      curnode != NULL;
		      curnode = curnode->next ) {

			tag_attribs = curnode->data;

			printf("    src=%d;type=%s;cat=%s",
					tag_attribs->spot_id,
					tag_attribs->type,
					tag_attribs->cat );

			if ( tag_attribs->group_type == spotlight_group_type_sales ) {
				printf(";qty=1;cost=0.00");
			}
			else {
				if ( tag_attribs->tag_method == spotlight_method_standard )
					printf(";ord=[rand]");
				else
					printf(";num=[rand];ord=1");
			}
			printf("\n");
		}
	}

	ll_destroy(&tag_list);

	return 0;
}
*/
