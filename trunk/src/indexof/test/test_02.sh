#!/bin/bash

test_name="input from file"

cat > $0.in << "END_INPUT"
f0	f1	f2
00	001	002
END_INPUT

index=`$wdir/indexof -l f2 -f $0.in -d '\t'`
expected=3

if [ $index -ne $expected ]; then
  echo "FAIL: $test_name - got $index instead of $expected"
  exit 1
else
  rm $0.in
  echo "PASS: $test_name"
  exit 0
fi
