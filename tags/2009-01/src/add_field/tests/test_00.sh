test_number=00
description="insert using indexes, new label specified"

subtests=('' '-f 0' '-f 1' '-f 2' '-f 4' '-f -1')
subtest_expect=('test.first.expect' 'test.first.expect' 'test.first.expect'
                'test.second.expect' 'test.last.expect' 'test.last.expect')
subtest_desc=('default' 'first specified as 0' 'first specified as 1'
              'middle' 'last specified as index' 'last specified as -1' )

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
