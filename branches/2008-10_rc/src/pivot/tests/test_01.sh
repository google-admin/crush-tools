test_number=01
description="multi-field pivot"

input=$test_dir/test_$test_number.in
output=$test_dir/test_$test_number.output
expected=$test_dir/test_$test_number.expected

$bin -k -f 1 -p 3,6 -v 4,5 $input > $output

if [ $? -ne 0 ] || [ "`diff -q $output $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $output
fi
