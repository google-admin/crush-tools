test_number=06
description="case-insensitive string collation (assumes local en_US.iso88591)"

# only run locale-sensitive tests when they can be expected to work
if [ "`which locale 2>/dev/null`" ] &&
   [ "`locale -a | grep en_US.iso88591`" ]; then
  export LC_ALL="en_US.iso88591"
  export LC_COLLATE="en_US.iso88591"
  locale_works=1
fi

if [ ! $locale_works ]; then
  test_status $test_number all "$description" SKIP
else
  for i in `seq 0 $((${#test_variants[*]} - 1))`; do
    outfile="$test_dir/test_$test_number.${test_variants[$i]}.actual"
    expected=$test_dir/test_$test_number.${test_variants[$i]}.expected
    $bin ${test_variants[$i]} \
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
fi
