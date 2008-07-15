#!/bin/bash

test_name="first field"

$wdir/subtotal -l 2 -k 1 -s 2,3 -d '\t' > $0.out << "END_INPUT"
f0	f1	f2
00	1	2
00	2	4
00	3	8
10	1	2
10	2	4
10	3	8
20	1	2
20	2	4
20	3	8
END_INPUT

cat > $0.expected << "END_EXPECT"
f0	f1	f2
00	1	2
00	2	4
00	3	8
	6	14

10	1	2
10	2	4
10	3	8
	6	14

20	1	2
20	2	4
20	3	8
	6	14
END_EXPECT

if [ "`diff -q $0.expected $0.out`" ]; then
  echo "FAIL: $test_name"
  exit 1
else
  rm "$0.expected" "$0.out"
  echo "PASS: $test_name"
  exit 0
fi
