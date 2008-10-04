test_number=01
description="formula using labels"

infile="$test_dir/test.txt"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -d ';' -e '[Metric-0] / [Metric-1]' -b '0.0' $infile > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" XFAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
