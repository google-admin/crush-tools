test_number=05
description="single-character labels"

infile="$test_dir/test.txt"
outfile="$test_dir/test_$test_number.actual"
expected="$test_dir/test_$test_number.expected"

cat $infile | sed -e 's/Metric-0/A/; s/Metric-1/B/' |
  $bin -p -d ';' -e '[A] / [B]' -b '0.0' > "$outfile"

if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm "$outfile"
fi
