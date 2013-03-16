test_number=11
description="whole file aggregation, no keys"


expected="$test_dir/test_$test_number.expected"
outfile="$test_dir/test_$test_number.actual"


$bin -p -L \
     -S Numeric-1,Numeric-2 \
     -C Numeric-1,Numeric-2 \
     -A Numeric-1,Numeric-2 \
     "$test_dir/test.in" "$test_dir/test.in2" \
     > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
