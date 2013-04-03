test_number=06
description="insert without placeholders"

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

$bin -s "INSERT INTO crush_test (Title, Pages) VALUES ('Snow Crash', '468')"
output=`$bin -s "SELECT Pages from crush_test where Title = 'Snow Crash'"`
if [ $? -ne 0 ] || [ $output -ne 468 ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
