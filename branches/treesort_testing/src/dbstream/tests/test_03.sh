test_number=03
description="insert"

perl -MDBD::DBM -e 'exit 0;' 2>/dev/null
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description" SKIP
  continue
fi

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

echo "Snow Crash,468" |
  $bin -s "INSERT INTO crush_test (Title, Pages) VALUES (?, ?)"
output=`$bin -s "SELECT Pages from crush_test where Title = 'Snow Crash'"`
if [ $? -ne 0 ] || [ $output -ne 468 ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
