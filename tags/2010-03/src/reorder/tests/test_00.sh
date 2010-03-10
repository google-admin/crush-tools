test_number=00
subtest=0
description="empty input"

output=`echo -n '' | $bin -f 1 2>&1`
if [ $? -eq 0 ] && [ "$output" = "" ]; then
  test_status $test_number $subtest "$description" PASS
else
  test_status $test_number $subtest "$description" FAIL
fi
