test_number=03
description="expression using labels"

infile="$test_dir/imps_clks.log"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -d ';' -e 'sprintf \"%.3f\", [Metric-0] / [Metric-1] * 100' \
     -b 'N/A' $infile > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" XFAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
