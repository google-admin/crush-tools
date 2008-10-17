test_number=00
description="select, no header"

perl -MDBD::DBM -e 'exit 0;' 2>/dev/null
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description" SKIP
  continue
fi

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

output=`$bin -s "SELECT * from crush_test where Title = 'Anathem'"`
if [ $? -ne 0 ] || [ "$output" != 'Anathem,935' ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
