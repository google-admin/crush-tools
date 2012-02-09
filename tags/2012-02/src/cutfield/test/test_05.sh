test_number=05
description="correctly err on missing field without segmentation fault"

expected=$test_dir/test_$test_number.expected
input=$test_dir/test_$test_number.in
cat > $input << "END_INPUT"
f0,f2
00,01
END_INPUT

cat > $expected << "END_EXPECT"
END_EXPECT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
errput=$test_dir/test_$test_number.$subtest.err
$bin -F f3 -d , $input > $output 2>$errput
if [ $? -ne 1 ] || [ "`diff -q $expected $output`" ] || [ ! -s $errput ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm $output $errput
fi

test $has_error || rm $input $expected 
