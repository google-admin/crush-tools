test_number=01
description="multi-key, multi-sum"

infile=$test_dir/test.in
outfile=$test_dir/test_$test_number.out
expected=$test_dir/test_$test_number.expected

subtest=1
$bin -p -k 1,2 -s 3,4 $infile > $outfile
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm "$outfile"
fi

subtest=2
$bin -K Text-1,Text-2 -S Numeric-1,Numeric-2 $infile > $outfile
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm "$outfile"
fi
