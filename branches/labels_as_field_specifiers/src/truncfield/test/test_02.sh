test_number=02
description="middle field"

input=$test_dir/test_$test_number.in
expected=$test_dir/test_$test_number.expected

cat > $input << "END_INPUT"
f0	f1	f2
00	01	02
10	11	12
20	21	22
END_INPUT

cat > $expected << "END_EXPECT"
f0		f2
00		02
10		12
20		22
END_EXPECT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
$bin -f 2 -d '\t' $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description (indexes)" FAIL
  has_error=1
else
  rm "$output"
  test_status $test_number 1 "$description (indexes)" PASS
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -F f1 -d '\t' $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description (labels)" FAIL
  has_error=1
else
  rm "$output"
  test_status $test_number 1 "$description (labels)" PASS
fi

if [ ! $has_error ]; then
  rm $input $expected
fi
