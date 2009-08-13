test_number=02
description="trailing input files"

subtest=0

output=`$bin -s 1,2 -d , < $test_dir/test.in0 2>&1`
if [ $? -eq 0 ] && [ "$output" = "02,01,03,04,05" ]; then
  test_status $test_number $subtest "$description (stdin)" PASS
else
  test_status $test_number $subtest "$description (stdin)" FAIL
fi

subtest=1

$bin -d , -f 1,3-5 $test_dir/test.in0 $test_dir/test.in1 \
  > $test_dir/test_$test_number.$subtest.out 2>&1

if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.$subtest.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (file)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (file)" PASS
fi
