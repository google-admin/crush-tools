test_number=03
description="insert with placeholders"

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

echo "Snow Crash,468" |
  $bin -s "INSERT INTO crush_test (Title, Pages) VALUES (?, ?)"
output=`$bin -s "SELECT Pages from crush_test where Title = 'Snow Crash'"`
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description (bad exit code)" FAIL
elif [ "$output" != 468 ]; then
  test_status $test_number 1 "$description (bad output: $output)" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
