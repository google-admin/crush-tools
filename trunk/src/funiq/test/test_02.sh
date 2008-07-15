#!/bin/bash

test_name="middle field without dups"

$wdir/funiq -f 2 -d '\t' > $0.out << "END_INPUT"
f0	f1	f2
00	001	002
00	002	004
00	003	008
10	111	112
10	112	114
10	113	118
20	221	222
20	222	224
20	223	228
END_INPUT

cat > $0.expected << "END_EXPECT"
f0	f1	f2
00	001	002
00	002	004
00	003	008
10	111	112
10	112	114
10	113	118
20	221	222
20	222	224
20	223	228
END_EXPECT

if [ "`diff -q $0.expected $0.out`" ]; then
  echo "FAIL: $test_name"
  exit 1
else
  rm "$0.expected" "$0.out"
  echo "PASS: $test_name"
  exit 0
fi
