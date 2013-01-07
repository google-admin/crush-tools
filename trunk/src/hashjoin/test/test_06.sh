test_number=06
description="empty input file"

infile=$test_dir/test_06_test.in
dimfile="$test_dir/dimension_header.log"

echo -n '' | $bin -k 1,2 -l 1,2 -j 3,4 -f $dimfile -

if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description (stdin)" FAIL
else
  test_status $test_number 1 "$description (stdin)" PASS
fi

:> $infile
echo -n '' | $bin -k 1,2 -l 1,2 -j 3,4 -f $dimfile $infile

if [ $? -ne 0 ]; then
  test_status $test_number 2 "$description (file)" FAIL
else
  test_status $test_number 2 "$description (file)" PASS
fi

rm $infile
