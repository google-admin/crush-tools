test_number=03
description="last field"

expected=$test_dir/test_$test_number.expected
input=$test_dir/test_$test_number.in

cat > $input << "END_INPUT"
f0	f1	f2
00	01	02
10	11	12
20	21	22
END_INPUT

cat > $expected << "END_EXPECT"
f0	f1
00	01
10	11
20	21
END_EXPECT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
$bin -f 3 $input > $output 
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -F f2 $input > $output 
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm $output
fi

test $has_error || rm $input $expected
