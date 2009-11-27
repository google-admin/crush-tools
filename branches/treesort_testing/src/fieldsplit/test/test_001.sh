test_number=001
description="preserve order during user-supplied transformations"

rm -f $test_dir/$test_number/*.actual
$bin -f 1 -s '.actual' \
     -x 's/^(\d+)-(\d+)-\d+/$1-$2/' \
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
