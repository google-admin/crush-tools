test_number=01
description="multiple labels filter: key1 and value1"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -p -K key1,value1 "$test_dir/test-filter.in" "$test_dir/test-1.in" "$test_dir/test-2.in" > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
