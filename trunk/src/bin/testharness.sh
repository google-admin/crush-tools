#!/bin/bash

if [ ! "$1" ] || [ "$1" = "-h" ]; then
  echo "usage: $0 <config-file>" >&2
  exit 1
fi

if [ ! -e "$1" ]; then
  echo "$0: \"$1\": no such file" >&2
  exit 1
fi


pass=0
fail=0
xfail=0
skip=0

# report on the results of a test and tally the failures
# TODO(jeremy): write a log file containing commands that have a FAIL result
function test_status {
  local test_num="$1"
  local subtest="$2"
  local description="$3"
  local result="$4"

  echo "$result: $test_num.$subtest: $description"
  case "$result" in
    PASS)  ((pass++));;
    FAIL)  ((fail++));;
    SKIP)  ((skip++));;
    XFAIL) ((xfail++));;
    *)    echo "UNRECOGNIZED RESULT: $result"; exit;;
  esac
}

cd `dirname "$1"`
. `basename "$1"`

# make it easier to check pipeline exit codes on stdin tests
set pipefail

for test_file in $test_files; do
  . $test_file
done

cat << END_SUMMARY
=====================================================
test results for $application:
  PASSED:  $pass 
  FAILED:  $fail unexpected, $xfail expected
  SKIPPED: $skip
=====================================================
END_SUMMARY

if [ $fail -gt 0 ]; then
  exit 1
fi
exit 0
