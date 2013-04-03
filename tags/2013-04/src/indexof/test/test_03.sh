test_number=03
description="input from string"

expected=3
index=`$bin -l f2 -s "f0	f1	f2"`

if [ $? -ne 0 ] || [ $index -ne 3 ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
