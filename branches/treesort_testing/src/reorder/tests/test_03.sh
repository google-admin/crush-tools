test_number=03
description="single input file, label-specified fields"

# input on stdin
subtest=0
cat $test_dir/test_labeled.in |
  $bin -d , -F Field-2,Field-0 \
  > $test_dir/test_$test_number.$subtest.out 2>&1

if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (stdin)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (stdin)" PASS
fi

# input from file
subtest=1
$bin -d , -F Field-2,Field-0 $test_dir/test_labeled.in \
  > $test_dir/test_$test_number.$subtest.out 2>&1
if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (file)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (file)" PASS
fi

