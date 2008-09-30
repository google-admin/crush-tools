test_number=01
description="input on stdin"

expected=1
index=`$bin -l f0 -d '\t' << "END_INPUT"
f0	f1	f2
00	001	002
END_INPUT`

if [ $? -ne 0 ] || [ $index -ne $expected ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
