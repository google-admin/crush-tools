test_number=02
description="header preserved"

expected=$test_dir/test_$test_number.expected

subtest=1
output=$test_dir/test_$test_number.$subtest.out
$bin -p -f 4 [0-9] $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (index)" FAIL
else
  test_status $test_number $subtest "$description (index)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -F field3 [0-9] $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (label)" FAIL
else
  test_status $test_number $subtest "$description (label)" PASS
  rm $output
fi
