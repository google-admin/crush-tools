test_number=01
description="first field"

input=$test_dir/test_$test_number.in
expected=$test_dir/test_$test_number.expected

cat > $expected << "END_EXPECT"
	f1	f2
	01	02
	11	12
	21	22
END_EXPECT

cat > $input << "END_INPUT"
f0	f1	f2
00	01	02
10	11	12
20	21	22
END_INPUT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
$bin -f 1 -d '\t' $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
  has_error=1
else
  rm "$output"
  test_status $test_number $subtest "$description (indexes)" PASS
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -F f0 -d '\t' $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
  has_error=1
else
  rm "$output"
  test_status $test_number $subtest "$description (labels)" PASS
fi

if [ ! $has_error ]; then
  rm "$input" "$expected" 
fi
