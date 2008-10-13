test_number=01
description="inverted basic"

output=$test_dir/test_$test_number.out
expected=$test_dir/test_$test_number.expected

$bin -v -f 4 [0-9] $input > $output

if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $output
fi
