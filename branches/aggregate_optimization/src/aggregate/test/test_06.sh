test_number=06
description="labels as field specifiers with 2 input files"

subtest_desc=("default labels" "auto-labels" "user labels")
subtest_opts=(
''
'-L'
'-l "Sum-1	Sum-2	Count-1	Count-2	Average-1	Average-2"'
)

for subtest in `seq 0 2`; do
  expected="$test_dir/test_$test_number.$subtest.expected"
  outfile="$test_dir/test_$test_number.$subtest.actual"

  eval $bin -p \
       -K Text-1,Text-2 \
       -S Numeric-1,Numeric-2 \
       -C Numeric-1,Numeric-2 \
       -A Numeric-1,Numeric-2 \
       "${subtest_opts[$subtest]}" \
       "$test_dir/test.in" "$test_dir/test.in2" \
       > "$outfile"

  if [ $? -ne 0 ] ||
     [ "`diff -q $outfile $expected`" ]; then
    test_status $test_number $subtest \
                "$description (${subtest_desc[$subtest]})" FAIL
  else
    test_status $test_number $subtest \
                "$description (${subtest_desc[$subtest]})" PASS
    rm "$outfile"
  fi
done
