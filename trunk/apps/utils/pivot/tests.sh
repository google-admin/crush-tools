#!/bin/bash

wdir=`dirname $0`

subtests=(00 01)
datadir=$wdir/tests
export DELIMITER="\t"

tests=("-k -f 1 -p 2 -v 3" "-k -f 1 -p 3,6 -v 4,5")
test_names=("minimal" "multi-field pivot")
errors=0

for i in `seq 0 $(( ${#tests[*]} - 1 ))`; do

	echo "" # just put an empty line for readability

	optstring=`echo ${tests[$i]} | sed 's/\s//g'`

	for t in ${subtests[*]}; do

		infile=$datadir/test_${t}.in
		expected=$datadir/test_${t}.${optstring}.expected
		out=$datadir/test_${t}.${optstring}.out

		if [ -e $expected ]; then

			echo -n "test $i - \"${test_names[$i]}\" (sub test ${t}): "

			$wdir/pivot ${tests[$i]} "$infile" > "$out"

			if [ "`diff -q "$out" "$expected"`" ]; then
				(( errors++ ))
				echo "failed"
				echo "		see $out"
			else
				echo "passed"
				rm $out
			fi
		fi

	done

done

echo -e "\n$errors errors\n"
