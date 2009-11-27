test_number=01
description="two column sort"

subtest_desc=("DD,BB"
							"3,1 using index"
							"^DD,BB"
							"^DD,^BB"
							"4,^2 -p")
subtest_opts=(
'-K DD,BB'
'-k 3,1 -p'
'-K ^DD,BB'
'-K ^DD,^BB'
'-k 4,^2 -p'
)

for subtest in `seq 0 4`; do
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
