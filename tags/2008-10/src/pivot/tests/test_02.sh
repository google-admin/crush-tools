test_number=02
description="segfault bug fix"

input=$test_dir/test_$test_number.in
output=$test_dir/test_$test_number.output
expected=$test_dir/test_$test_number.expected

$bin -k -f 1-10 -p 20,21 -v 11,12,18,19 $input > $output

if [ $? -ne 0 ] || [ "`diff -q $output $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $output
fi
