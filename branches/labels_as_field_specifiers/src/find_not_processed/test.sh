#!/bin/bash

has_error=0
ls . | grep -v test.sh > logfile.tmp
echo logfile.tmp >> logfile.tmp

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

echo "test.sh" >> logfile.tmp

f=`./find_not_processed -l logfile.tmp -d . -g '*'`
if [ "$f" ]; then
  echo "FAIL: test 3: $f"
  has_error=1
else
  echo "PASS: test 3"
fi

f=`./find_not_processed -l logfile.tmp -d . -r '^[^\.].*'`
if [ "$f" ]; then
  echo "FAIL: test 4: $f"
  has_error=1
else
  echo "PASS: test 4"
fi

if [ $has_error -eq 0 ]; then rm logfile.tmp; fi
exit $has_error
