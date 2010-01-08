test_number=03
description="copy from existing field"

subtests=('-c 3 -f -1'
          '-C f3 -f -1'
          '-c 3 -f -1 -l NEW')
subtest_expect=("test_${test_number}.0.expect"
                "test_${test_number}.1.expect"
                "test_${test_number}.2.expect")
subtest_desc=('pure copy by index'
              'pure copy by label'
              'copy with different label')

input=$test_dir/test.in

for subtest in `seq 0 $(( ${#subtests[*]} - 1 ))`; do
  output=$test_dir/test_$test_number.$subtest.out
  expected=$test_dir/${subtest_expect[$subtest]}
  $bin ${subtests[$subtest]} $input > $output
  if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
    test_status $test_number $subtest \
                "$description (${subtest_desc[$subtest]})" FAIL
  else
    test_status $test_number $subtest \
                "$description (${subtest_desc[$subtest]})" PASS
    rm $output
  fi
done

