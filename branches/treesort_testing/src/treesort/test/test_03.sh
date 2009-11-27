test_number=03
description="all column sort"

subtest_desc=("AA,BB,CC,DD"
							"BB,AA,CC,DD"
							"DD,AA,CC,BB"
							"3,2,4,1 indexes"
							'AA,^BB,CC,^DD'
							'AA,^BB,^CC,DD'
							'^3,1,^4,2 indexes'
							'^3,^4,^2,^1 indexes'
							)
subtest_opts=(
'-K AA,BB,CC,DD'
'-K BB,AA,CC,DD'
'-K DD,AA,CC,BB'
'-k 3,2,4,1 -p'
'-K AA,^BB,CC,^DD'
'-K AA,^BB,^CC,DD'
'-k ^3,1,^4,2 -p'
'-k ^3,^4,^2,^1 -p'
)

for subtest in `seq 0 7`; do
  expected="$test_dir/test_$test_number.$subtest.expected"
  outfile="$test_dir/test_$test_number.$subtest.actual"

  $bin ${subtest_opts[$subtest]} \
       "$test_dir/test2.in" \
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
