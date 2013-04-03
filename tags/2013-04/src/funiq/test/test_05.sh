test_number=05
description="0-byte input"

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
f0	f1	f2
00	001	002
10	111	112
20	221	222
END_EXPECT

subtest=1
output=$test_dir/test_$test_number.$subtest.out
echo -n '' | $bin -f 1 > $output
if [ $? -ne 0 ] || [ -s $output ]; then
  test_status $test_number $subtest "$description (stdin)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (stdin)" PASS
  rm $output
fi

subtest=2
output=$test_dir/test_$test_number.$subtest.out
$bin -f 1 <(cat /dev/null) > $output
if [ $? -ne 0 ] || [ -s $output ]; then
  test_status $test_number $subtest "$description (single input)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (single input)" PASS
  rm $output
fi

subtest=3
output=$test_dir/test_$test_number.$subtest.out
$bin -F f0 <(cat /dev/null) $input > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (empty first file)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (empty first file)" PASS
  rm $output
fi

subtest=4
output=$test_dir/test_$test_number.$subtest.out
$bin -F f0 $input <(cat /dev/null) > $output
if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number $subtest "$description (empty second file)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (empty second file)" PASS
  rm $output
fi

subtest=5
output=$test_dir/test_$test_number.$subtest.out
$bin -F f0 $input.nonesuch > $output 2>&1
if [ $? -eq 0 ] || [ ! -s $output ]; then
  test_status $test_number $subtest "$description (bad input file)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (bad input file)" PASS
  rm $output
fi





if [ ! $has_error ]; then
  rm $expected $input
fi
