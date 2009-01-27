test_number=02
description="one key, multi-average (header preserved)"

outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -p -k 1 -s 3,4 -a 3,4 "$test_dir/test.in" > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
