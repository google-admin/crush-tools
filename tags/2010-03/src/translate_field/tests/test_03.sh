test_number=03
description="external mappings"

infile="$test_dir/test.in"
mapping="$test_dir/test_$test_number.map"
expected="$test_dir/test_$test_number.expected"

cat > $mapping << 'END'
$mapping{a} = 'AYE';
$mapping{e} = 'EEE';
$mapping{code} = 'HACK';
END

subtest=1
outfile="$test_dir/test_$test_number.$subtest.actual"
$bin -f 1 -e $mapping "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (index)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (index)" PASS
  rm "$outfile"
fi

subtest=2
outfile="$test_dir/test_$test_number.$subtest.actual"
$bin -F Field-0 -e $mapping "$test_dir/test.in" > "$outfile"
if [ $? -ne 0 ] ||
   [ "`diff -q $outfile $expected`" ]; then
  test_status $test_number $subtest "$description (label)" FAIL
  has_error=1
else
  test_status $test_number $subtest "$description (label)" PASS
  rm "$outfile"
fi

if [ ! $has_error ]; then
  rm "$mapping"
fi
