test_number=01
description="multi-field pivot"

input=$test_dir/test_$test_number.in
expected=$test_dir/test_$test_number.expected

subtest=1
output=$test_dir/test_$test_number.$subtest.output
$bin -k -f 1 -p 3,6 -v 4,5 $input > $output
if [ $? -ne 0 ] || [ "`diff -q $output $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.output
$bin -F ID -P Attr-1,Event-ID -A Quantity,Revenue $input > $output
if [ $? -ne 0 ] || [ "`diff -q $output $expected`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm $output
fi
