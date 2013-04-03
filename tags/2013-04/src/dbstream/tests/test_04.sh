test_number=04
description="select with placeholders"

$test_dir/setup.pl || {
  test_status $test_number 1 "$description (setup failed)" SKIP
  continue
}

output="`echo -e 'APUE 2e\nAnathem' |
           $bin -s 'SELECT Pages from crush_test where Title = ?' |
           tr '\n' ,`"
if [ $? -ne 0 ]; then
  test_status $test_number 1 "$description (bad exit code)" FAIL
elif [ "$output" != '960,935,' ]; then
  test_status $test_number 1 "$description (bad output: $output)" FAIL
else
  test_status $test_number 1 "$description" PASS
fi
rm $test_dir/crush_test.*
