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
#include <crush/linklist.h>
#include <crush/ffutils.h>
#include "unittest.h"


int test_fields_in_line() {
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

  unittest_has_error = 0;
  n = fields_in_line(TL0, TD0);
  ASSERT_LONG_EQ(TE0, n, "fields_in_line: correct return value (1)");

  n = fields_in_line(TL1, TD1);
  ASSERT_LONG_EQ(TE1, n, "fields_in_line: correct return value (2)");

  n = fields_in_line(TL2, TD2);
  ASSERT_LONG_EQ(TE2, n, "fields_in_line: correct return value (3)");

  n = fields_in_line(TL3, TD3);
  ASSERT_LONG_EQ(TE3, n, "fields_in_line: correct return value (4)");

  return unittest_has_error;
}

int test_get_line_field(void) {
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

  unittest_has_error = 0;

	n = get_line_field(buffer, TL0, bufsz, TF0, TD);
  ASSERT_INT_EQ(TE0_ret, n, "get_line_field: correct return value (1a)");
  ASSERT_STR_EQ(TE0_buffer, buffer, "get_line_field: correct field value (1a)");

	n = get_line_field(buffer, TL1, bufsz, TF0, TD);
  ASSERT_INT_EQ(TE0_ret, n, "get_line_field: correct return value (1b)");
  ASSERT_STR_EQ(TE0_buffer, buffer, "get_line_field: correct field value (1b)");

	n = get_line_field(buffer, TL0, bufsz, TF1, TD);
  ASSERT_INT_EQ(TE1_ret, n, "get_line_field: correct return value (2a)");
  ASSERT_STR_EQ(TE1_buffer, buffer, "get_line_field: correct field value (2a)");

	n = get_line_field(buffer, TL1, bufsz, TF1, TD);
  ASSERT_INT_EQ(TE1_ret, n, "get_line_field: correct return value (2b)");
  ASSERT_STR_EQ(TE1_buffer, buffer, "get_line_field: correct field value (2b)");

	n = get_line_field(buffer, TL0, bufsz, TF2, TD);
  ASSERT_INT_EQ(TE2_ret, n, "get_line_field: correct return value (3a)");
  ASSERT_STR_EQ(TE2_buffer, buffer, "get_line_field: correct field value (3a)");

	n = get_line_field(buffer, TL1, bufsz, TF2, TD);
  ASSERT_INT_EQ(TE2_ret, n, "get_line_field: correct return value (3b)");
  ASSERT_STR_EQ(TE2_buffer, buffer, "get_line_field: correct field value (3b)");

	n = get_line_field(buffer, TL0, bufsz, TF3, TD);
  ASSERT_INT_EQ(TE3_ret, n, "get_line_field: correct return value (4a)");
  ASSERT_STR_EQ(TE3_buffer, buffer, "get_line_field: correct field value (4a)");

	n = get_line_field(buffer, TL1, bufsz, TF3, TD);
  ASSERT_INT_EQ(TE3_ret, n, "get_line_field: correct return value (4b)");
  ASSERT_STR_EQ(TE3_buffer, buffer, "get_line_field: correct field value (4b)");

  return unittest_has_error;
}


int test_copy_field(void) {
  char *buffer = NULL;
  size_t buffer_sz = 0;
  int retval;
  const char *line = "A||BB|CCC\n";

  unittest_has_error = 0;

  /* out-of-bounds field number */
  retval = copy_field(line, &buffer, &buffer_sz, 5, "|");
  ASSERT_TRUE(retval == -1, "copy_field: Negative return value expected");
  ASSERT_TRUE(buffer_sz == 0, "copy_field: No buffer size change expected");
  ASSERT_TRUE(buffer == NULL, "copy_field: No alloc expected");

  retval = copy_field(line, &buffer, &buffer_sz, 1, "|");
  ASSERT_TRUE(retval == 0, "copy_field: Zero return value expected");
  ASSERT_TRUE(buffer_sz == 0, "copy_field: No buffer size change expected");
  ASSERT_TRUE(buffer == NULL, "copy_field: No alloc expected");

  retval = copy_field(line, &buffer, &buffer_sz, 0, "|");
  ASSERT_TRUE(retval == 1, "copy_field: Return value 1 expected");
  ASSERT_TRUE(buffer_sz == 2, "copy_field: Update buffer size");
  ASSERT_STR_EQ(buffer, "A", "copy_field: Alloc and populate buffer");

  retval = copy_field(line, &buffer, &buffer_sz, 2, "|");
  ASSERT_TRUE(retval == 2, "copy_field: Return value 2 expected");
  ASSERT_TRUE(buffer_sz == 3, "copy_field: Update buffer size");
  ASSERT_STR_EQ(buffer, "BB", "copy_field: Relloc and populate buffer");

  retval = copy_field(line, &buffer, &buffer_sz, 3, "|");
  ASSERT_TRUE(retval == 3, "copy_field: Return value 3 expected");
  ASSERT_TRUE(buffer_sz == 4, "copy_field: Update buffer size");
  ASSERT_STR_EQ(buffer, "CCC", "copy_field: Relloc and populate buffer");

  retval = copy_field(line, &buffer, &buffer_sz, 0, "|");
  ASSERT_TRUE(retval == 1, "copy_field: Return value 1 expected");
  ASSERT_TRUE(buffer_sz == 4, "copy_field: No update to buffer size");
  ASSERT_STR_EQ(buffer, "A", "copy_field: Populate buffer (no realloc)");

  return unittest_has_error;
}


int test_get_line_pos(void) {
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

  unittest_has_error = 0;

	n = get_line_pos(TL0, TF0, TD0, &start, &end);
  ASSERT_INT_EQ(TE0_ret, n, "get_line_pos: correct return value (1)");
  ASSERT_TRUE(start == TE0_start, "get_line_pos: correct start position (1)");
  ASSERT_TRUE(end == TE0_end, "get_line_pos: correct end position (1)");

	n = get_line_pos(TL1, TF1, TD1, &start, &end);
  ASSERT_INT_EQ(TE1_ret, n, "get_line_pos: correct return value (2)");
  ASSERT_TRUE(start == TE1_start, "get_line_pos: correct start position (2)");
  ASSERT_TRUE(end == TE1_end, "get_line_pos: correct end position (2)");

	n = get_line_pos(TL2, TF2, TD2, &start, &end);
  ASSERT_INT_EQ(TE2_ret, n, "get_line_pos: correct return value (3)");
  ASSERT_TRUE(start == TE2_start, "get_line_pos: correct start position (3)");
  ASSERT_TRUE(end == TE2_end, "get_line_pos: correct end position (3)");

	n = get_line_pos(TL3, TF3, TD3, &start, &end);
  ASSERT_INT_EQ(TE3_ret, n, "get_line_pos: correct return value (4)");
  ASSERT_TRUE(start == TE3_start, "get_line_pos: correct start position (4)");
  ASSERT_TRUE(end == TE3_end, "get_line_pos: correct end position (4)");

  return unittest_has_error;
}

int test_field_start(void) {
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

  unittest_has_error = 0;

	ret = field_start(TL, TF0, TD);
  ASSERT_STR_EQ(TE0, ret, "field_start: correct return value (1)");

	ret = field_start(TL, TF1, TD);
  ASSERT_STR_EQ(TE1, ret, "field_start: correct return value (2)");

	ret = field_start(TL, TF2, TD);
  ASSERT_STR_EQ(TE2, ret, "field_start: correct return value (3)");

	ret = field_start(TL, TF3, TD);
  /* The return value should be NULL, so don't use STR_EQ */
  ASSERT_TRUE(ret == TE3, "field_start: correct return value (4)");

  return unittest_has_error;
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

  unittest_has_error = 0;

	ret = mdyhms_datecmp(TD0_a, TD0_b);
  ASSERT_INT_EQ(TE0, ret, "mdyhms_datecmp: correct return value (1)");

	ret = mdyhms_datecmp(TD1_a, TD1_b);
  ASSERT_INT_EQ(TE1, ret, "mdyhms_datecmp: correct return value (2)");

	ret = mdyhms_datecmp(TD2_a, TD2_b);
  ASSERT_INT_EQ(TE2, ret, "mdyhms_datecmp: correct return value (3)");

  return unittest_has_error;
}

int test_chomp(void) {
  int n_errors = 0;

  char T0_pre[] = "hello world\n";
  char T0_post[] = "hello world";

  char T1_pre[] = "hello world\r\n";
  char T1_post[] = "hello world";

  char T2_pre[] = "hello world";
  char T2_post[] = "hello world";

  unittest_has_error = 0;

  chomp(T0_pre);
  ASSERT_STR_EQ(T0_pre, T0_post, "chomp: \\n linebreak");
  chomp(T1_pre);
  ASSERT_STR_EQ(T1_pre, T1_post, "chomp: \\r\\n linebreak");
  chomp(T2_pre);
  ASSERT_STR_EQ(T2_pre, T2_post, "chomp: no linebreak");

  return unittest_has_error;
}

int test_nextfile(void) {
  unittest_has_error = 0;
  ASSERT_TRUE(1, "nextfile: skip - not implemented.");
  return unittest_has_error;
}

int test_expand_chars(void) {
  char *TL0 = "\t";
  char TE0[] = { 0x09, 0x00 };

  char *TL1 = "no special chars";
  char *TE1 = "no special chars";

  char TL2[] = { 0x5c, 0x5c, 0x00 };  /* \\ */
  char TE2[] = { 0x5c, 0x00 };

  unittest_has_error = 0;

  expand_chars(TL0);
  ASSERT_STR_EQ(TL0, TE0, "expand_chars: tab expanded");
  expand_chars(TL1);
  ASSERT_STR_EQ(TL1, TE1, "expand_chars: no special chars");
  expand_chars(TL2);
  ASSERT_STR_EQ(TL2, TE2, "expand_chars: escaped backslash");

  return unittest_has_error;
}

int test_expand_nums(void) {
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

  unittest_has_error = 0;
  tmpstr = malloc(16);

  strcpy(tmpstr, TL0);
  n = expand_nums(tmpstr, &target, &target_size);
  ASSERT_LONG_EQ(TE0, n, "expand_nums: return value with list");

  strcpy(tmpstr, TL1);
  n = expand_nums(tmpstr, &target, &target_size);
  ASSERT_LONG_EQ(TE1, n, "expand_nums: return value with range)");

  strcpy(tmpstr, TL2);
  n = expand_nums(tmpstr, &target, &target_size);
  ASSERT_LONG_EQ(TE2, n, "expand_nums: return value with bad input");

  /* TL3 is null */
  n = expand_nums(TL3, &target, &target_size);
  ASSERT_LONG_EQ(TE3, n, "expand_nums: return value with null input");

  free(target);
  free(tmpstr);

  return unittest_has_error;
}


int test_expand_label_list(void) {
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

  unittest_has_error = 0;

  retval = expand_label_list(T0_labels, line, delim, &array, &array_sz);
  ASSERT_INT_EQ(T0_retval, retval, "expand_label_list: return value (0)");

  for (i = 0; i < retval; i++) {
    ASSERT_INT_EQ(T0_elems[i], array[i], "expand_label_list: field index (0)");
  }

  retval = expand_label_list(T1_labels, line, delim, &array, &array_sz);
  ASSERT_INT_EQ(T1_retval, retval, "expand_label_list: return value (1)");

  retval = expand_label_list(T2_labels, line, delim, &array, &array_sz);
  ASSERT_INT_EQ(T2_retval, retval, "expand_label_list: return value (2)");
  for (i = 0; i < retval; i++) {
    ASSERT_INT_EQ(T2_elems[i], array[i], "expand_label_list: field index (2)");
  }

  free(array);
  return unittest_has_error;
}


int test_field_str() {
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

  unittest_has_error = 0;

  /* test case: field exists, multi-char delimiter */
  n = field_str(TV0, TL0, TD0);
  ASSERT_INT_EQ(TE0, n, "field_str: return value (0)");
  n = field_str(TV1, TL1, TD1);
  ASSERT_INT_EQ(TE1, n, "field_str: return value (1)");
  n = field_str(TV2, TL2, TD2);
  ASSERT_INT_EQ(TE2, n, "field_str: return value (2)");
  n = field_str(TV3, TL3, TD3);
  ASSERT_INT_EQ(TE3, n, "field_str: return value (3)");
  n = field_str(TV4, TL4, TD4);
  ASSERT_INT_EQ(TE4, n, "field_str: return value (4)");
  n = field_str(TV5, TL5, TD5);
  ASSERT_INT_EQ(TE5, n, "field_str: return value (5)");
  n = field_str(TV6, TL6, TD6);
  ASSERT_INT_EQ(TE6, n, "field_str: return value (6)");
  n = field_str(TV7, TL7, TD7);
  ASSERT_INT_EQ(TE7, n, "field_str: return value (7)");
  n = field_str(TV8, TL8, TD8);
  ASSERT_INT_EQ(TE8, n, "field_str: return value (8)");
  n = field_str(TV9, TL9, TD9);
  ASSERT_INT_EQ(TE9, n, "field_str: return value (9)");

  return unittest_has_error;
}

int main(int argc, char *argv[]) {
  int errs = 0;
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
  errs += test_field_str();
  errs += test_copy_field();
  if (errs)
    return EXIT_FAILURE;
  return EXIT_SUCCESS;
}
