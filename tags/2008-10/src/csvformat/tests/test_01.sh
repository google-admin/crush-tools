test=01
sub_test=0
description="no linebreak at end of file."
output=`echo -n "hello${DELIMITER}world" | $bin 2>&1`
if [ "$output" != '"hello","world"' ]; then
  test_status $test $sub_test "$description" FAIL
else
  test_status $test $sub_test "$description" PASS
fi
