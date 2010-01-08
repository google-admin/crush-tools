test_number=01
description="defaults"

left=$test_dir/test_$test_number-full.txt
right=$test_dir/test_$test_number-delta.txt
expected=$test_dir/test_$test_number.expected

output=$test_dir/test_$test_number.0.out
$bin $left $right > $output

if [ $? -ne 0 ] ||
   [ "`diff -q $output $expected`" ]; then
  test_status $test_number 0 "$description (${subtests[0]})" FAIL
else
  test_status $test_number 0 "$description (${subtests[0]})" PASS
  rm "$output"
fi

output=$test_dir/test_$test_number.1.out
cat $left | $bin - $right > $output

if [ $? -ne 0 ] ||
   [ "`diff -q $output $expected`" ]; then
  test_status $test_number 1 "$description (${subtests[1]})" FAIL
else
  test_status $test_number 1 "$description (${subtests[1]})" PASS
  rm "$output"
fi

output=$test_dir/test_$test_number.2.out
cat $right | $bin $left - > $output

if [ $? -ne 0 ] ||
   [ "`diff -q $output $expected`" ]; then
  test_status $test_number 2 "$description (${subtests[2]})" FAIL
else
  test_status $test_number 2 "$description (${subtests[2]})" PASS
  rm "$output"
fi

