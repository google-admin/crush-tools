test_number=02
description="input from file"

input="$test_dir/test_$test_number.in"
cat > "$input" << END_INPUT
f0	f1	f2
00	001	002
END_INPUT

expected=3
index=`$bin -l f2 -f "$input"`

if [ $? -ne 0 ] || [ $index -ne $expected ]; then
  test_status $test_number 1 "$description" FAIL
else
  rm $input
  test_status $test_number 1 "$description" PASS
fi
