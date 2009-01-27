test_number=04
description="handle blank key fields correctly"

infile=$test_dir/test_$test_number.in
outfile=$test_dir/test_$test_number.out
expected=$test_dir/test_$test_number.expected

cat > $infile << END_TEST_INPUT
Key-Header	Value-Header
	1
	1
Key-0	1
Key-0	1
END_TEST_INPUT

cat > $expected << END_TEST_EXPECT
Key-Header	Value-Header
	2
Key-0	2
END_TEST_EXPECT

subtest=1
$bin -p -k 1 -c 2 $infile > $outfile
if [ $? -ne 0 ] || [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
fi

subtest=1
$bin -K Key-Header -C Value-Header $infile > $outfile
if [ $? -ne 0 ] || [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (indexes)" FAIL
else
  test_status $test_number $subtest "$description (indexes)" PASS
  rm "$infile" "$outfile" "$expected"
fi
