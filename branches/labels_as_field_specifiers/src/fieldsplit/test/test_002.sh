test_number=002
description="fail when missing required params"

$bin >/dev/null 2>&1

if [ $? -eq 0 ]; then
  test_status $test_number 0 "$description" FAIL
else
  test_status $test_number 0 "$description" PASS
fi
