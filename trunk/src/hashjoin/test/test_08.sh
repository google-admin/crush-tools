test_number=08
description="join by index with header-only input"

infile="$test_dir/input_header.log"
dimfile="$test_dir/dimension_header.log"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

head -1 $infile | $bin -k 1,2 -l 1,2 -j 3,4 -f $dimfile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
