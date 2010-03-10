test_number=03
description="one-key join by index w/out headers"

# There are 2 records in the dimension file with a first field of 1.  The
# 2nd should override the first.

infile="$test_dir/input_no_header.log"
dimfile="$test_dir/dimension_no_header.log"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

$bin -k 1 -l 1 -j 3,4 -f $dimfile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
