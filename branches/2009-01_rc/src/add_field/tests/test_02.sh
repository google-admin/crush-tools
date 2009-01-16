test_number=02
description="insert after label-specified location"

subtests=('-A f1' '-A f2' '-A f3')
subtest_expect=('test.second.expect' 'test.third.expect' 'test.last.expect')
subtest_desc=('after first field' 'after second field' 'after last field')

input=$test_dir/test.in

for subtest in `seq 0 $(( ${#subtests[*]} - 1 ))`; do
  output=$test_dir/test_$test_number.$subtest.out
  expected=$test_dir/${subtest_expect[$subtest]}
  $bin ${subtests[$subtest]} -l 'NEW' -v 00 $input > $output
  if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
    test_status $test_number $subtest \
                "$description (${subtest_desc[$subtest]})" FAIL
  else
    test_status $test_number $subtest \
                "$description (${subtest_desc[$subtest]})" PASS
    rm $output
  fi
done

