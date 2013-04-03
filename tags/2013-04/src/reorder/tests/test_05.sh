test_number=05
description="very long fields"

subtest=0
$bin -d , -m 2,3 $test_dir/test_$test_number.in \
  > $test_dir/test_$test_number.$subtest.out 2>&1

if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.$subtest.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (move)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (move)" PASS
fi

subtest=1
$bin -d , -s 1,2 $test_dir/test_$test_number.in \
  > $test_dir/test_$test_number.$subtest.out 2>&1
if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.$subtest.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (swap)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (swap)" PASS
fi

subtest=2
$bin -d , -f 3,2,1 $test_dir/test_$test_number.in \
  > $test_dir/test_$test_number.$subtest.out 2>&1
if [ $? -ne 0 ] ||
   [ "`diff -q $test_dir/test_$test_number.$subtest.expected \
            $test_dir/test_$test_number.$subtest.out`" ]; then
  test_status $test_number $subtest "$description (field-list)" FAIL
else
  rm $test_dir/test_$test_number.$subtest.out
  test_status $test_number $subtest "$description (field-list)" PASS
fi
