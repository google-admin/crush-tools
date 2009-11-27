test_number=02
description="two-key join by index w/out header in the dim file"

infile="$test_dir/input_header.log"
dimfile="$test_dir/dimension_no_header.log"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -k 1,2 -l 1,2 -j 3,4 -L "Mine-0${DELIMITER}Mine-1" -f $dimfile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
