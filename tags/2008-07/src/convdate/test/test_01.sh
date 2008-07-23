#!/bin/bash

test_name="defaults"

$wdir/convdate -d '\t' > $0.out << "END_TEST"
Date	Field-0	Field-1
10-11-2008-16:32:08	hello	world
10-11-2008-16:32:08	hello	world
END_TEST

cat > $0.expected << "END_EXPECT"
Date	Field-0	Field-1
2008-10-11-16:32:08	hello	world
2008-10-11-16:32:08	hello	world
END_EXPECT

if [ "`diff -q $0.expected $0.out`" ]; then
  echo "FAIL: $test_name"
  exit 1
else
  rm "$0.expected" "$0.out"
  echo "PASS: $test_name"
  exit 0
fi
