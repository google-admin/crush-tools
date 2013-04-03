test_number=04
description="multiple fields, different column order"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -p -a 1,2 -b 2,3 -f "$test_dir/test-filter.in" "$test_dir/test-3.in" "$test_dir/test-4.in" > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
