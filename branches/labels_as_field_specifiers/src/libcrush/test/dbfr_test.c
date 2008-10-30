#include <stdlib.h>
#include <string.h>
#include "dbfr.h"

#define DIE(s, ...) \
  do { \
    fprintf(stderr, "%s: (%s:%d): " s, \
            getenv("_"), __FILE__,__LINE__, ##__VA_ARGS__); \
    exit(EXIT_FAILURE); \
  } while (0)

#define TEST_FILENAME "test_input.log"
#define LINES_IN_TEST_FILE 10

char failure_reason[256];

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
  if (! reader) {
    strcpy(failure_reason, "dbfr_open() returned null.");
    return 1;
  }
  if (reader->current_line != NULL) {
    strcpy(failure_reason, "current_line was not initialized to null");
    return 1;
  }
  if (reader->line_no != 0) {
    strcpy(failure_reason, "line_no was not initialized to zero");
    return 1;
  }
  if (strcmp(reader->next_line, "this is line 1\n") != 0) {
    strcpy(failure_reason, "next_line was not initialized properly");
    return 1;
  }
  dbfr_close(reader);
  return 0;
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
  if (reader != NULL) {
    strcpy(failure_reason, "dbfr_init succeeded on a file opened for writing.");
    return 1;
  }
  return 0;
}

/* tests all conditions which should hold after reading the first line 
   of the file. */
int test_dbfr_getline_1() {
  dbfr_t *reader = dbfr_open(TEST_FILENAME);
  ssize_t retval;
  if (! reader) {
    strcpy(failure_reason, "dbfr_open() returned null.");
    return 1;
  }
  retval = dbfr_getline(reader);
  if (retval != strlen("this is line 1\n")) {
    sprintf(failure_reason, "dbfr_getline() returned %d.", retval);
    return 1;
  }
  if (reader->line_no != 1) {
    strcpy(failure_reason, "line_no was not incremented properly");
    return 1;
  }
  if (strcmp(reader->current_line, "this is line 1\n") != 0) {
    strcpy(failure_reason, "current_line was not initialized properly");
    return 1;
  }
  if (strcmp(reader->next_line, "this is line 2\n") != 0) {
    strcpy(failure_reason, "next_line was not initialized properly");
    return 1;
  }
  dbfr_close(reader);
  return 0;
}

/* tests all conditions which should hold when reading to the end of the file. */
int test_dbfr_getline_2() {
  dbfr_t *reader = dbfr_open(TEST_FILENAME);
  ssize_t retval;
  int i = 0;
  char expected[32];

  if (! reader) {
    strcpy(failure_reason, "dbfr_open() returned null.");
    return 1;
  }

  while (i < LINES_IN_TEST_FILE) {
    i++;
    dbfr_getline(reader);
    if (i != reader->line_no) {
      sprintf(failure_reason, "line number set to %d after %d reads",
              reader->line_no, i);
      return 1;
    }
  }

  if (reader->next_line != NULL) {
    strcpy(failure_reason, "next_line not NULL at last line");
    return 1;
  }

  sprintf(expected, "this is line %d\n", LINES_IN_TEST_FILE); 
  if (strcmp(reader->current_line, expected) != 0) {
    sprintf(failure_reason, "current_line not set as expected");
    return 1;
  }

  retval = dbfr_getline(reader);
  if (retval > 0) {
    sprintf(failure_reason,
            "dbfr_getline() returned %d where EOF should be.", retval);
    return 1;
  }
  if (! reader->eof) {
    strcpy(failure_reason, "dbfr_t.eof not not set");
    return 1;
  }

  if (reader->current_line == NULL) {
    strcpy(failure_reason, "current_line was NULLified after EOF");
    return 1;
  }

  dbfr_close(reader);
  return 0;
}

int main (int argc, char *argv[]) {
  int has_failures = 0;
  int has_error;

  has_error = setup();
  if (has_error)
    DIE("failure during test setup");

#define RUN_TEST(fn) \
  do { \
    has_error = fn(); \
    if (has_error) { \
      has_failures = 1; \
      printf("FAIL: %s - %s\n", #fn, failure_reason); \
    } else { \
      printf("PASS: %s\n", #fn); \
    } \
  } while (0)


  RUN_TEST(test_dbfr_open);
  RUN_TEST(test_dbfr_init);
  RUN_TEST(test_dbfr_getline_1);
  RUN_TEST(test_dbfr_getline_2);

  teardown();
  return 0;
}





