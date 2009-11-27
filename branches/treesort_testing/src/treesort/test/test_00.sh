test_number=00
description="one column sort"

subtest_desc=("first column"
							"second column using index"
							"first column reversed"
							"second column reversed with index")
subtest_opts=(
'-K AA'
'-k 2 -p'
'-K ^AA'
'-k ^2 -p'
)

for subtest in `seq 0 3`; do
  expected="$test_dir/test_$test_number.$subtest.expected"
  outfile="$test_dir/test_$test_number.$subtest.actual"

  $bin ${subtest_opts[$subtest]} \
       "$test_dir/test.in" \
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
