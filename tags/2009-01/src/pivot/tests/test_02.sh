test_number=02
description="segfault bug fix"

input=$test_dir/test_$test_number.in
expected=$test_dir/test_$test_number.expected

subtest=1
output=$test_dir/test_$test_number.$subtest.output
$bin -k -f 1-10 -p 20,21 -v 11,12,18,19 $input > $output
if [ $? -ne 0 ] || [ "`diff -q $output $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.output
$bin -F Name,Blank-0,Blank-1,ID,Met-0,Met-1,Met-2,Met-3,Met-4,Met-5 \
     -P Met-15,Data \
     -A Met-6,Met-7,Met-13,Met-14 $input > $output
if [ $? -ne 0 ] || [ "`diff -q $output $expected`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm $output
fi
