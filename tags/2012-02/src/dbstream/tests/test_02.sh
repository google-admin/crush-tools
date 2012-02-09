test_number=02
description="update with placeholders"

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

echo "1000" |
  $bin -s "UPDATE crush_test SET Pages = ? WHERE Title = 'Anathem'"
output=`$bin -s "SELECT Pages from crush_test where Title = 'Anathem'"`
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description (bad exit code)" FAIL
elif [ "$output" != 1000 ]; then
  test_status $test_number 1 "$description (bad output: $output)" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
