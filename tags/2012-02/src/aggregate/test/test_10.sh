test_number=10
description="missing record fields"

subtests=("-K Text-1,Text-2 -S Numeric-1"
          "-K Text-1,Text-2 -S Numeric-2"
          "-K Text-1,Text-2 -S Numeric-1,Numeric-2")

for subtest in `seq 1 ${#subtests[*]}`; do
  outfile="$test_dir/test_${test_number}_${subtest}.out"
  $bin ${subtests[$(( $subtest - 1 ))]} $test_dir/test.in3 > $outfile 2>&1
  retval=$?
  if [ $retval -eq 0 ]; then
    test_status $test_number $subtest "$description - wrong exit code." FAIL
  elif [ $retval -eq 139 ]; then
    test_status $test_number $subtest "$description - segfault." FAIL
  else
    test_status $test_number $subtest "$description." PASS
    rm "$outfile"
  fi
done
