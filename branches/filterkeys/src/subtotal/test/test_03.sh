test_number=03
description="no header line, but with leading data"

expected=$test_dir/test_$test_number.expected
output=$test_dir/test_$test_number.out

cat > $expected << "END_EXPECT"
Here is some free-form text

00	1	2
00	2	4
00	3	8
	6	14

10	1	2
10	2	4
10	3	8
	6	14

20	1	2
20	2	4
20	3	8
	6	14
END_EXPECT

$bin -l 3 -k 1 -s 2,3 > $output << "END_INPUT"
Here is some free-form text

00	1	2
00	2	4
00	3	8
10	1	2
10	2	4
10	3	8
20	1	2
20	2	4
20	3	8
END_INPUT

if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$expected" "$output"
fi
