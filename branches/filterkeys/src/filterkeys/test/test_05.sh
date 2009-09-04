test_number=05
description="stdin filter-file"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

cat "$test_dir/test-filter-2.in" |
$bin -p -a 3,1 -b 1,3 -f - "$test_dir/test-3.in" "$test_dir/test-4.in" > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
