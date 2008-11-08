test_number=04
description="correctly handle empty fields"

expected=$test_dir/test_$test_number.expected
input=$test_dir/test_$test_number.in
cat > $input << "END_INPUT"
f0,f1,f2
00,,02
10,,12
20,21,
30,31,
END_INPUT

cat > $expected << "END_EXPECT"
f1,f2
,02
,12
21,
31,
END_EXPECT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
$bin -f 1 -d , $input > $output 
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -F f0 -d , $input > $output 
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm $output
fi

test $has_error || rm $input $expected
