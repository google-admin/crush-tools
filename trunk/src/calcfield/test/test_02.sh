test_number=02
description="expression using indexes"

infile="$test_dir/imps_clks.log"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -p -d 'þ' -e 'sprintf "%.3f", [2] / [3] * 100' \
     -b 'N/A' $infile > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
