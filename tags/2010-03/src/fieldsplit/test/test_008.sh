test_number=008
description="split on last field in line, label as field specifier"

rm -f $test_dir/$test_number/*.actual
DELIMITER_SAVE="$DELIMITER"
unset DELIMITER
$bin -k -F Data-2 -s '.actual' \
     -p "$test_dir/$test_number" \
     "$test_dir/002-data.txt"
export DELIMITER="$DELIMITER_SAVE"
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
