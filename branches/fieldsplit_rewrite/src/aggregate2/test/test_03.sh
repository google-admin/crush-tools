test_number=03
description="multi-key, float sum"

infile=$test_dir/test.in
outfile=$test_dir/test_$test_number.out
expected=$test_dir/test_$test_number.expected

subtest=1
$bin -p -k 1,2 -s 5 $infile > $outfile
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm "$outfile"
fi

subtest=2
$bin -K Text-1,Text-2 -S Numeric-3 $infile > $outfile
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm "$outfile"
fi
