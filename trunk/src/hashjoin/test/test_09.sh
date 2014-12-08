test_number=09
description="multiple input files"

subtest=1
subtest_desc="with header"
infile="$test_dir/input_header.log"
dimfile="$test_dir/dimension_header.log"
outfile="$test_dir/test_$test_number.$subtest.actual"
expected="$test_dir/test_$test_number.$subtest.expected"

$bin -K 'Field-0,Field-1' -J 'Field-2,Field-3' -f $dimfile $infile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description ($subtest_desc)" FAIL
else
  test_status $test_number $subtest "$description ($subtest_desc)" PASS
  rm "$outfile"
fi

subtest=2
subtest_desc="no header"
infile="$test_dir/input_no_header.log"
dimfile="$test_dir/dimension_no_header.log"
outfile="$test_dir/test_$test_number.$subtest.actual"
expected="$test_dir/test_$test_number.$subtest.expected"

$bin -k 1,2 -l 1,2 -j 3,4 -f $dimfile $infile $infile \
  > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description ($subtest_desc)" FAIL
else
  test_status $test_number $subtest "$description ($subtest_desc)" PASS
  rm "$outfile"
fi
