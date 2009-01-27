test_number=04
description="placement of calculated field"

infile="$test_dir/test.txt"

subtests=('-p -i 1' '-p -r 1' '-B Text2' '-A Metric-1')
subtest_desc=('insert first' 'replace first' 'before label' 'after label')

for i in `seq 0 $(( ${#subtests[*]} - 1 ))`; do
  expected="$test_dir/test_$test_number.$i.expected"
  outfile="$test_dir/test_$test_number.$i.actual"
  $bin ${subtests[$i]} -d ';' -e '[3] / [2]' -b '0.0' $infile > "$outfile"
  if [ $? -ne 0 ] ||
     [ "`diff -q $outfile $expected`" ]; then
    test_status $test_number $i "$description (${subtest_desc[$i]})" FAIL
  else
    test_status $test_number $i "$description (${subtest_desc[$i]})" PASS
    rm "$outfile"
  fi
done
