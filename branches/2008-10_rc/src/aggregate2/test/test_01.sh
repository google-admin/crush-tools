test_number=01
description="multi-key, multi-sum"

infile=$test_dir/test.in
outfile=$test_dir/test_$test_number.out
expected=$test_dir/test_$test_number.expected

$bin -p -k 1,2 -s 3,4 $infile > $outfile

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
