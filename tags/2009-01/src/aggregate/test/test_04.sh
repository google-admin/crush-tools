test_number=04
description="auto-label aggregated field"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -p -k 1 -s 3 -c 1 -a 4 -L "$test_dir/test.in" > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
