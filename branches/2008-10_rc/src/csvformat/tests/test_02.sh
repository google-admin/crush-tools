test=02
description="quote-escaping"

sub_test=0
output=`echo "say \"hello\"${DELIMITER}world" | $bin 2>&1`
if [ "$output" != '"say ""hello""","world"' ]; then
  test_status $test $sub_test "$description (escaped)" FAIL
else
  test_status $test $sub_test "$description (escaped)" PASS
fi

sub_test=1
output=`echo "say \"hello\"${DELIMITER}world" | $bin -n 2>&1`
if [ "$output" != '"say "hello"","world"' ]; then
  test_status $test $sub_test "$description (not escaped)" FAIL
else
  test_status $test $sub_test "$description (not escaped)" PASS
fi
