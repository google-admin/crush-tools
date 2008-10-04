test_number=04
description="correctly handle empty fields"

expected=$test_dir/test_$test_number.expected
output=$test_dir/test_$test_number.out

cat > $expected << "END_EXPECT"
f1,f2
,02
,12
21,
31,
END_EXPECT

$bin -f 1 -d , > $output << "END_INPUT"
f0,f1,f2
00,,02
10,,12
20,21,
30,31,
END_INPUT

if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $expected $output
fi
