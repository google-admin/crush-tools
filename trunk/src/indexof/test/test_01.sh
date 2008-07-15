#!/bin/bash

test_name="input on stdin"

expected=1
index=`$wdir/indexof -l f0 -d '\t' << "END_INPUT"
f0	f1	f2
00	001	002
END_INPUT`

if [ $index -ne $expected ]; then
  echo "FAIL: $test_name - got $index instead of $expected"
  exit 1
else
  echo "PASS: $test_name"
  exit 0
fi
