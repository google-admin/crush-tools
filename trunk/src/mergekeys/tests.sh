#!/bin/bash

# test 00 - 1:1 matching between both files
# test 01 - both files have just a header
# test 02 - file A has just a header; B has data
# test 03 - A has data; file B has just a header
# test 04 - last line of A is greater than last line of B
# test 05 - last line of A is less than last line of B
# test 06 - case-insensitive string collation (assumes local en_US.iso88591)
# test 07 - one:many relationship between keys in A and B


wdir=`dirname $0`

# only run locale-sensitive tests when they can be expected to work
if [ "`which locale 2>/dev/null`" ] &&
   [ "`locale -a | grep en_US.iso88591`" ]; then
  export LC_ALL="en_US.iso88591"
  export LC_COLLATE="en_US.iso88591"
  locale_works=1
fi

subtests=(00 01 02 03 04 05 06 07 08)
datadir=$wdir/tests
test_delim='\t'

tests=("" "-l" "-r" "-i")
test_names=("full outer" "left outer" "right outer" "inner")
errors=0

for i in `seq 0 $(( ${#tests[*]} - 1 ))`; do

  echo "" # just put an empty line for readability

  optstring=`echo ${tests[$i]} | sed 's/\s//g'`

  for t in ${subtests[*]}; do

    left=$datadir/test_${t}.a
    right=$datadir/test_${t}.b
    expected=$datadir/test_${t}.${optstring}.expected
    out=$datadir/test_${t}.${optstring}.out

    if [ -e $expected ]; then

      echo -n "test $i - \"${test_names[$i]}\" (sub test ${t}): "

      if [ "$t" = "06" ] && [ ! "$locale_works" ]; then
        echo "skipped"
        continue
      fi

      $wdir/mergekeys -d "$test_delim" $optstring $left $right > $out.1
      cat $left | $wdir/mergekeys -d "$test_delim" $optstring - $right > $out.2
      cat $right | $wdir/mergekeys -d "$test_delim" $optstring $left - > $out.3

      if [ "`diff -q $out.1 $expected`" ] || [ "`diff -q $out.2 $expected`" ] || [ "`diff -q $out.3 $expected`" ]; then
        (( errors++ ))
        echo "failed"
        for ((j=1; j<=3; j++)); do
          if [ "`diff -q $out.$j $expected`" ]; then echo "          see $out.$j"; fi
        done
      else
        echo "passed"
        rm $out.*
      fi

    else # if [ -e $expected ]; then
      echo "creating $expected from $left, $right and options ${tests[$i]}"
      $wdir/mergekeys -d "$test_delim" $optstring $left $right > $expected
    fi
  done # subtests
done # tests

echo -e "\n$errors errors\n"

if [ $errors -gt 0 ]; then
  exit 1
fi
exit 0
