#!/bin/bash

test_name="first field"

$wdir/truncfield -f 1 -d '\t' > $0.out << "END_INPUT"
f0	f1	f2
00	01	02
10	11	12
20	21	22
END_INPUT

cat > $0.expected << "END_EXPECT"
	f1	f2
	01	02
	11	12
	21	22
END_EXPECT

if [ "`diff -q $0.expected $0.out`" ]; then
  echo "FAIL: $test_name"
  exit 1
else
  rm "$0.expected" "$0.out"
  echo "PASS: $test_name"
  exit 0
fi
