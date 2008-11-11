test_number=01
description="insert before label-specified location"

subtests=('-B f1' '-B f2' '-B f3')
subtest_expect=('test.first.expect' 'test.second.expect' 'test.third.expect')
subtest_desc=('before first field' 'before second field' 'before last field')

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

