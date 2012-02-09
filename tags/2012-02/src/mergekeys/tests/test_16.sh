test_number=16
description="empty fields"

outfile="$test_dir/test_$test_number.actual"
expected=$test_dir/test_$test_number.expected
$bin -d : -o "$outfile" \
     "$test_dir/test_$test_number.a" \
     "$test_dir/test_$test_number.b"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $i "$description" FAIL
else
  test_status $test_number $i "$description" PASS
  rm "$outfile"
fi
