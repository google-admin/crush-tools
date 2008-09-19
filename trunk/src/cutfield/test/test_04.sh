#!/bin/bash

test_name="correctly handle empty fields"

$wdir/cutfield -f 1 -d , > $0.out << "END_INPUT"
f0,f1,f2
00,,02
10,,12
20,21,
30,31,
END_INPUT

cat > $0.expected << "END_EXPECT"
f1,f2
,02
,12
21,
31,
END_EXPECT

if [ "`diff -q $0.expected $0.out`" ]; then
  echo "FAIL: $test_name"
  exit 1
else
  rm "$0.expected" "$0.out"
  echo "PASS: $test_name"
  exit 0
fi
