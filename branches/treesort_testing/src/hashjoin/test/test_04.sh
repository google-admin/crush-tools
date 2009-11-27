test_number=04
description="exit failure on undefined labels."

# There are 2 records in the dimension file with a first field of 1.  The
# 2nd should override the first.

infile="$test_dir/input_header.log"
dimfile="$test_dir/dimension_header.log"

subtest=1
$bin -K 'Bad-Field' -J Field-2 -f $dimfile $infile >/dev/null 2>&1

if [ $? -eq 0 ]; then
  test_status $test_number $subtest "$description (keys)" FAIL
else
  test_status $test_number $subtest "$description (keys)" PASS
fi

subtest=2
$bin -K 'Field-0' -J 'Bad-Field' -f $dimfile $infile >/dev/null 2>&1

if [ $? -eq 0 ]; then
  test_status $test_number $subtest "$description (dimensions)" FAIL
else
  test_status $test_number $subtest "$description (dimensions)" PASS
fi
