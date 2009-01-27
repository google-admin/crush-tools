test_number=04
description="single input file, label-specified swap/move"

subtests=('-m Field-2,Field-0' '-m Field-2,1' '-m 3,Field-0'\
          '-s Field-1,Field-0' '-s Field-2,1' '-s 1,Field-2') 
subtest_desc=('move: label,label' 'move: label,index' 'move: index,label'\
              'swap: label,label' 'swap: label,index' 'swap: index,label')

for i in `seq 0 $(( ${#subtests[*]} - 1 ))`; do
  $bin -d , ${subtests[$i]} $test_dir/test_labeled.in \
    > $test_dir/test_$test_number.$i.out 2>&1
  if [ $? -ne 0 ] ||
     [ "`diff -q $test_dir/test_$test_number.$i.expected \
              $test_dir/test_$test_number.$i.out`" ]; then
    test_status $test_number $i "$description (${subtest_desc[$i]})" FAIL
  else
    rm $test_dir/test_$test_number.$i.out
    test_status $test_number $i "$description (${subtest_desc[$i]})" PASS
  fi
done
