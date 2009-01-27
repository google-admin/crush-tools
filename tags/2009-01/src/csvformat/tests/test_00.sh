test=00
sub_test=0
description="preservation of linebreak style"

echo -ne "hello${DELIMITER}\nhello${DELIMITER}world\r\nbye${DELIMITER}now" |
  $bin 2>&1 \
  > $test_dir/test_${test}.out

if [ "`diff -q $test_dir/test_${test}.out $test_dir/test_${test}.expected`" ]
then
  test_status $test $sub_test "$description" FAIL
else
  rm $test_dir/test_${test}.out
  test_status $test $sub_test "$description" PASS
fi
