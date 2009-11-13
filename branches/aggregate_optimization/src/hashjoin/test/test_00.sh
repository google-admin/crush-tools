test_number=00
description="two-key join by label"

infile="$test_dir/input_header.log"
dimfile="$test_dir/dimension_header.log"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -K 'Field-0,Field-1' -J 'Field-2,Field-3' -f $dimfile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
