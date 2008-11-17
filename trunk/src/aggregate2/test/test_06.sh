test_number=06
description="auto-labels"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

subtest=1
$bin -k 1 -c 2 -s 3 -L "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm "$outfile"
fi

subtest=2
$bin -K Text-1 -C Text-2 -S Numeric-1 -L "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm "$outfile"
fi

