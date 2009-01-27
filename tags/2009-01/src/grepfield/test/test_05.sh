test_number=05
description="cr/lf EOL"

input_sav="$input"
input=$test_dir/test_$test_number.in

subtest=1
output=$test_dir/test_$test_number.$subtest.out
expected=$test_dir/test_$test_number.$subtest.expected
$bin -p -f 1 -v . $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (first field)" FAIL
else
  test_status $test_number $subtest "$description (first field)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
expected=$test_dir/test_$test_number.$subtest.expected
$bin -p -f 2 -v . $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (middle field)" FAIL
else
  test_status $test_number $subtest "$description (middle field)" PASS
  rm $output
fi

subtest=3
output=$test_dir/test_$test_number.$subtest.out
expected=$test_dir/test_$test_number.$subtest.expected
$bin -p -f 3 -v . $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (last field)" FAIL
else
  test_status $test_number $subtest "$description (last field)" PASS
  rm $output
fi

input="$input_sav"
