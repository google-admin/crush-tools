test_number=05
description="out of bounds field indexes"

infile="$test_dir/input_header.log"
dimfile="$test_dir/dimension_header.log"

subtest=1
outfile="$test_dir/test_$test_number.$subtest.actual"
expected="$test_dir/test_$test_number.$subtest.expected"
$bin -k 1,10 -l 1,2 -j 3,4 -f $dimfile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (data keys)" FAIL
else
  test_status $test_number $subtest "$description (data keys)" PASS
  rm "$outfile"
fi

subtest=2
outfile="$test_dir/test_$test_number.$subtest.actual"
expected="$test_dir/test_$test_number.$subtest.expected"
$bin -k 1,2 -l 1,10 -j 3,4 -f $dimfile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (dimension keys)" FAIL
else
  test_status $test_number $subtest "$description (dimension keys)" PASS
  rm "$outfile"
fi

subtest=3
outfile="$test_dir/test_$test_number.$subtest.actual"
expected="$test_dir/test_$test_number.$subtest.expected"
$bin -k 1,2 -l 1,2 -j 3,10 -f $dimfile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (keys)" FAIL
else
  test_status $test_number $subtest "$description (keys)" PASS
  rm "$outfile"
fi
