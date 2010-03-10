test_number=006
description="split into limited buckets"

rm -f $test_dir/$test_number/*.actual
$bin -f 2 -b 3 -s '.actual' \
     -p "$test_dir/$test_number" \
     "$test_dir/001-transform-key.txt"

if [ $? -ne 0 ]; then
  test_status $test_number 0 "$description" FAIL
  continue
fi

validate_output || {
  test_status $test_number 0 "$description" FAIL
  continue
}
test_status $test_number 0 "$description" PASS
rm $test_dir/$test_number/*.actual
