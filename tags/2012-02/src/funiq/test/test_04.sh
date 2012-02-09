test_number=04
description="-c option"

input=$test_dir/test_$test_number.in
expected=$test_dir/test_$test_number.expected

cat > $input << "END_INPUT"
f0	f1	f2
00	001	002
00	002	004
00	003	008
10	111	112
10	112	114
10	113	118
20	221	222
20	222	224
20	223	228
END_INPUT

cat > $expected << "END_EXPECT"
f0	f1	f2	1
00	001	002	3
10	111	112	3
20	221	222	3
END_EXPECT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
$bin -f 1 -c $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -F f0 -c $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (labels)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (labels)" PASS
  rm $output
fi

if [ ! $has_error ]; then
  rm $expected $input
fi
