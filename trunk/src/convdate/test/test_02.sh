#!/bin/bash

test_name="format spec"

$wdir/convdate -d '\t' -i "%Y-%m-%d-%H:%M:%S" -o "%Y-%m-%d" \
  > $0.out << "END_TEST"
Date	Field-0	Field-1
2008-10-11-16:32:08	hello	world
2008-10-11-16:32:08	hello	world
END_TEST

cat > $0.expected << "END_EXPECT"
Date	Field-0	Field-1
2008-10-11	hello	world
2008-10-11	hello	world
END_EXPECT

if [ "`diff -q $0.expected $0.out`" ]; then
  echo "FAIL: $test_name"
  exit 1
else
  rm "$0.expected" "$0.out"
  echo "PASS: $test_name"
  exit 0
fi
