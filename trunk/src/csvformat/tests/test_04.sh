test=04

sub_test=0
description="quote all"
output=`echo "hello${DELIMITER}world" | $bin -q all 2>&1`
if [ "$output" != '"hello","world"' ]; then
  test_status $test $sub_test "$description" FAIL
else
  test_status $test $sub_test "$description" PASS
fi

sub_test=1
description="quote none"
output=`echo "hello${DELIMITER}there, world" | $bin -q none 2>&1`
if [ "$output" != 'hello,there, world' ]; then
  test_status $test $sub_test "$description" FAIL
else
  test_status $test $sub_test "$description" PASS
fi

sub_test=2
description="quote minimal"
output=`echo "hello${DELIMITER}there, world" | $bin -q minimal 2>&1`
if [ "$output" != 'hello,"there, world"' ]; then
  test_status $test $sub_test "$description" FAIL
else
  test_status $test $sub_test "$description" PASS
fi

sub_test=3
description="quote nonnumeric"
output=`echo "hello${DELIMITER}1${DELIMITER}2.3${DELIMITER}-4${DELIMITER}world" |
          $bin -q nonnumeric 2>&1`
if [ "$output" != '"hello",1,2.3,-4,"world"' ]; then
  test_status $test $sub_test "$description" FAIL
else
  test_status $test $sub_test "$description" PASS
fi

sub_test=4
description="quote specific"
output=`echo "hello${DELIMITER}1${DELIMITER}2.3${DELIMITER}world" |
          $bin -f 2-3 2>&1`
if [ "$output" != 'hello,"1","2.3",world' ]; then
  test_status $test $sub_test "$description" FAIL
else
  test_status $test $sub_test "$description" PASS
fi

