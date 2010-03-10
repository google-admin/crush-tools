test_number=09
description="bad field labels"

subtests=("-K Nonesuch -c 1"
          "-K Text-1 -C Nonesuch"
          "-K Text-1 -S Nonesuch"
          "-K Text-1 -A Nonesuch"
          "-K Text-1 -N Nonesuch"
          "-K Text-1 -X Nonesuch")

for subtest in `seq 1 ${#subtests[*]}`; do
  outfile="$test_dir/test_${test_number}_${subtest}.out"
  $bin ${subtests[$(( $subtest - 1 ))]} $test_dir/test.in > $outfile 2>&1
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
