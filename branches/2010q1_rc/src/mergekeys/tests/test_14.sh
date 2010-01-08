# Verify honoring of one-to-many relationships when using user-specified
# key fields which are in different positions in the 2 files.
# Bug reported by Allister:
# http://groups.google.com/group/crush-tools/browse_thread/thread/a79eed347966721a

test_number=14
description="one:many with user-specified keys"

for i in `seq 0 $((${#test_variants[*]} - 1))`; do
  outfile="$test_dir/test_$test_number.${test_variants[$i]}.actual"
  expected=$test_dir/test_$test_number.expected
  $bin ${test_variants[$i]} \
       -d , -A key1,key2 -B key1,key2 \
       -o "$outfile" \
       "$test_dir/test_$test_number.a" \
       "$test_dir/test_$test_number.b"

  if [ $? -ne 0 ] ||
     [ "`diff -q $outfile $expected`" ]; then
    test_status $test_number $i "$description (${variant_desc[$i]})" FAIL
  else
    test_status $test_number $i "$description (${variant_desc[$i]})" PASS
    rm "$outfile"
  fi
done
