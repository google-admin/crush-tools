test_number=01
description="input on stdin"

subtest=0
cat $test_dir/test.in0 $test_dir/test.in1 |
  $bin -d , -m 2,3 \
  > $test_dir/test_$test_number.$subtest.out 2>&1

if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.$subtest.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (stdin)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (stdin)" PASS
fi

subtest=1
output=`$bin -s 1,2 -d , < $test_dir/test.in0 2>&1`
if [ $? -eq 0 ] && [ "$output" = "02,01,03,04,05" ]; then
  test_status $test_number $subtest "$description (file)" PASS
else
  test_status $test_number $subtest "$description (file)" FAIL
fi
