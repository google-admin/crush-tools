test_number=01
description="select with header"

perl -MDBD::DBM -e 'exit 0;' 2>/dev/null
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description" SKIP
  continue
fi

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

output=(`$bin -H -s "SELECT * from crush_test where Title = 'Anathem'"`)
if [ $? -ne 0 ] ||
   [ "${output[0]}" != 'Title,Pages' ] ||
   [ "${output[1]}" != 'Anathem,935' ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
