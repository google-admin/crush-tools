#!/bin/bash

has_error=0
ls . | grep -v test.sh > logfile.tmp
f=`./find_not_processed -l logfile.tmp -d . -g '*'`
if [ "$f" != "test.sh" ]; then
  echo "FAIL: test 1: $f"
  has_error=1
else
  echo "PASS: test 1"
fi

f=`./find_not_processed -l logfile.tmp -d . -r '^[^\.].*'`
if [ "$f" != "test.sh" ]; then
  echo "FAIL: test 2: $f"
  has_error=1
else
  echo "PASS: test 2"
fi

rm logfile.tmp
exit $has_error
