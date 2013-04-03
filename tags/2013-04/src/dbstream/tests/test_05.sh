test_number=05
description="update without placeholders"

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

$bin -s "UPDATE crush_test SET Pages = 1000 WHERE Title = 'Anathem'"
output=`$bin -s "SELECT Pages from crush_test where Title = 'Anathem'"`
if [ $? -ne 0 ] || [ $output -ne 1000 ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
