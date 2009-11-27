test_number=02
description="three column sort"

subtest_desc=("CC,BB,DD" 
							"AA,BB,CC"
							"DD,CC,BB"
							"4,2,1 with indexes"
							'^AA,BB,CC'
							'^AA,^BB,CC'
							'^AA,^BB,^CC'
							'1,^2,^3 with indexes'
							'AA,BB,^CC'
							'1,^2,3 with indexes'
							)
subtest_opts=(
'-K CC,BB,DD'
'-K AA,BB,CC'
'-K DD,CC,BB'
'-k 4,2,1 -p'
'-K ^AA,BB,CC'
'-K ^AA,^BB,CC'
'-K ^AA,^BB,^CC'
'-k 1,^2,^3 -p'
'-K AA,BB,^CC'
'-k 1,^2,3 -p'
)

for subtest in `seq 0 9`; do
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
