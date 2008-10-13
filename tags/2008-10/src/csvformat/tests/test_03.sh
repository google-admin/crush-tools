test=03
description="input/output options"

sub_test=0
echo "hello${DELIMITER}world" |
  $bin -o $test_dir/test_$test.$subtest.out 2>&1

if [ "`diff -q $test_dir/test_$test.$subtest.out \
            $test_dir/test_${test}.expected`" ]
then
  test_status $test $sub_test "$description (-o)" FAIL
else
  rm $test_dir/test_$test.$subtest.out
  test_status $test $sub_test "$description (-o)" PASS
fi

sub_test=1
$bin -i $test_dir/test_${test}.in -o $test_dir/test_$test.$subtest.out 2>&1

if [ "`diff -q $test_dir/test_$test.$subtest.out \
            $test_dir/test_${test}.expected`" ]
then
  test_status $test $sub_test "$description (-i and -o)" FAIL
else
  rm $test_dir/test_$test.$subtest.out
  test_status $test $sub_test "$description (-i and -o)" PASS
fi

sub_test=2
output=`$bin -i $test_dir/test_${test}.in 2>&1`
expected=`cat $test_dir/test_${test}.expected`
if [ "$output" != "$expected" ]
then
  test_status $test $sub_test "$description (-i)" FAIL
else
  test_status $test $sub_test "$description (-i)" PASS
fi
