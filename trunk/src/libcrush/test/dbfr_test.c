#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crush/dbfr.h>
#include "unittest.h"

#define DIE(s, ...) \
  do { \
    fprintf(stderr, "%s: (%s:%d): " s, \
            getenv("_"), __FILE__,__LINE__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
  } while (0)

#define TEST_FILENAME "test_input.log"
#define LINES_IN_TEST_FILE 10

int setup() {
  int i;
  FILE *test_input = fopen(TEST_FILENAME, "w");
  if (! test_input)
    return 1;
  for (i=1; i <= LINES_IN_TEST_FILE; i++)
    fprintf(test_input, "this is line %d\n", i);
  fclose(test_input);
  return 0;
}

void teardown() {
  unlink(TEST_FILENAME);
}

/* tests all conditions which should hold after a file is opened but
   before it is read.
 */
int test_dbfr_open() {
  dbfr_t *reader = dbfr_open(TEST_FILENAME);
  unittest_has_error = 0;
  ASSERT_TRUE(reader != NULL, "dbfr_open: return non-null");
  if (reader == NULL) {
    return 1;
  }
  ASSERT_TRUE(reader->current_line == NULL,
              "dbfr_open: initialize current_line to NULL");
  ASSERT_LONG_EQ(0, reader->line_no,
                 "dbfr_open: initialized line_no to zero");
  ASSERT_STR_EQ("this is line 1\n", reader->next_line,
                "dbfr_open: read next_line");
  dbfr_close(reader);
  return unittest_has_error;
}

/* dbfr_init should fail on file pointers opened for writing.  dbfr_open()
 * is a wrapper around dbfr_init(), so the positive case is already tested.
 */
int test_dbfr_init() {
  dbfr_t *reader;
  FILE *f = fopen("test_input_2.log", "w");
  unlink("test_input_2.log");
  reader = dbfr_init(f);
  fclose(f);
  unittest_has_error = 0;
  ASSERT_TRUE(reader == NULL,
              "dbfr_init: fail on file opened for writing.");
  return unittest_has_error;
}

/* tests all conditions which should hold after reading the first line 
   of the file. */
int test_dbfr_getline_1() {
  dbfr_t *reader = dbfr_open(TEST_FILENAME);
  ssize_t retval;
  unittest_has_error = 0;
  ASSERT_TRUE(reader != NULL, "dbfr_open: return non-null");
  if (! reader) {
    return 1;
  }
  retval = dbfr_getline(reader);
  ASSERT_LONG_EQ(strlen("this is line 1\n"), retval,
                 "dbfr_getline: return string length");
  ASSERT_LONG_EQ(1, reader->line_no,
                 "dbfr_getline: increment line_no");
  ASSERT_STR_EQ("this is line 1\n", reader->current_line,
                "dbfr_getline: initialize current_line");
  ASSERT_STR_EQ("this is line 2\n", reader->next_line,
                "dbfr_getline: initialize next_line");
  dbfr_close(reader);
  return unittest_has_error;
}

/* tests all conditions which should hold when reading to the end of the file. */
int test_dbfr_getline_2() {
  dbfr_t *reader = dbfr_open(TEST_FILENAME);
  ssize_t retval;
  int i = 0;
  char expected[80];
  unittest_has_error = 0;

  ASSERT_TRUE(reader != NULL, "dbfr_open: return non-null");
  if (! reader) {
    return 1;
  }

  while (i < LINES_IN_TEST_FILE) {
    i++;
    dbfr_getline(reader);
    ASSERT_LONG_EQ(i, reader->line_no, "dbfr_getline: line_no incremented");
  }

  ASSERT_TRUE(reader->current_line != NULL, "current_line not NULL at EOF");
  ASSERT_TRUE(reader->next_line == NULL, "next_line NULL at EOF");

  sprintf(expected, "this is line %d\n", LINES_IN_TEST_FILE); 
  ASSERT_STR_EQ(expected, reader->current_line, "current_line intact at EOF");

  retval = dbfr_getline(reader);
  ASSERT_LONG_GT(0, retval, "dbfr_getline() returns < 0 at EOF");
  ASSERT_TRUE(reader->eof, "dbfr_t.eof set at EOF");

  dbfr_close(reader);
  return 0;
}

int main (int argc, char *argv[]) {
  int has_failures = 0;

  if (setup() != 0) {
    fprintf(stderr, "(%s:%d): %s",
            __FILE__, __LINE__, "setup failed.");
    exit(EXIT_FAILURE);
  }

  has_failures += test_dbfr_open();
  has_failures += test_dbfr_init();
  has_failures += test_dbfr_getline_1();
  has_failures += test_dbfr_getline_2();

  teardown();
  if (has_failures)
    return 1;
  return 0;
}
