test_number=05
description="user-specified agg-field labels"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

subtest=1
$bin -k 1 -s 3 -l Sum-Field "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm "$outfile"
fi

subtest=2
$bin -K Text-1 -S Numeric-1 -l Sum-Field "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description (labels)" FAIL
else
  test_status $test_number 1 "$description (labels)" PASS
  rm "$outfile"
fi

