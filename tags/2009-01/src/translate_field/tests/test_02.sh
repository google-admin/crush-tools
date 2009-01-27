test_number=02
description="translate last field"

infile="$test_dir/test.in"
expected="$test_dir/test_$test_number.expected"

subtest=1
outfile="$test_dir/test_$test_number.$subtest.actual"
$bin -f 3 -m "c=C" "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (index)" FAIL
else
  test_status $test_number $subtest "$description (index)" PASS
  rm "$outfile"
fi

subtest=2
outfile="$test_dir/test_$test_number.$subtest.actual"
$bin -F Field-2 -m "c=C" "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (label)" FAIL
else
  test_status $test_number $subtest "$description (label)" PASS
  rm "$outfile"
fi
