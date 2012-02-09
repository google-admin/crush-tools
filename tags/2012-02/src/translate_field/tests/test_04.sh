test_number=04
description="use expressions"

infile="$test_dir/test.in"
expected="$test_dir/test_$test_number.expected"
subtest=0

function ok {
  local _label=$1; shift
  ((subtest++))
  outfile="$test_dir/test_$test_number.$subtest.actual"
  eval $* "$test_dir/test.in" > "$outfile"
  if [ $? -ne 0 ] ||
    [ "`diff -q $outfile $expected`" ]; then
    test_status $test_number $subtest "$description ($_label)" FAIL
  else
    test_status $test_number $subtest "$description ($_label)" PASS
    rm "$outfile"
  fi
}

ok index $bin -f 1 -x "s/CODE/hack/i" -x "s/a/123/"
ok label $bin -F Field-0 -x "s/CODE/hack/i" -x "s/a/123/"
ok index $bin -f 1 -x "s/CODE/hack/i" -x "s/a/123/" -x "s/1/XXX/" -x "s/X+/1/"