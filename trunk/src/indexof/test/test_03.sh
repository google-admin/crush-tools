#!/bin/bash

test_name="input from string"

index=`$wdir/indexof -l f2 -s "f0	f1	f2" -d '\t'`
expected=3

if [ $index -ne 3 ]; then
  echo "FAIL: $test_name - got $index instead of $expected"
  exit 1
else
  echo "PASS: $test_name"
  exit 0
fi
