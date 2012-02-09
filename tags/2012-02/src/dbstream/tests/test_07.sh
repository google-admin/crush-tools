test_number=07
description="delete without placeholders"

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

$bin -s "DELETE from crush_test WHERE Title = 'Anathem'"
output=`$bin -s "SELECT Pages from crush_test where Title = 'Anathem'"`
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description (bad exit code)" FAIL
elif [ "$output" ]; then
  test_status $test_number 1 "$description (row not deleted.)" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
