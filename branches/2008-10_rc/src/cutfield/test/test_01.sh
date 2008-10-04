test_number=01
description="first field"

expected=$test_dir/test_$test_number.expected
output=$test_dir/test_$test_number.out

cat > $expected << "END_EXPECT"
f1	f2
01	02
11	12
21	22
END_EXPECT

$bin -f 1 > $output << "END_INPUT"
f0	f1	f2
00	01	02
10	11	12
20	21	22
END_INPUT

if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $expected $output
fi
