#!/bin/bash

wdir=`dirname $0`

subtests=(00 01)
datadir=$wdir/tests
test_delim='\t'

tests=(
	"\$left \$right"
	"- \$right"
	"\$left -"
)

errors=0

for i in `seq 0 $(( ${#tests[*]} - 1 ))`; do

	echo "" # just put an empty line for readability

	for t in ${subtests[*]}; do

		left=$datadir/test_${t}-full.txt
		right=$datadir/test_${t}-delta.txt
		expected=$datadir/test_${t}.expected
		out=$datadir/test_${t}.out

		eval "optstring=\"${tests[$i]}\""

		if [ -e $expected ]; then

			echo -n "test $i - (sub test ${t}): "

			case $i in
				0) $wdir/deltaforce -d "$test_delim" -o $out $optstring;;
				1) cat $left | $wdir/deltaforce -d "$test_delim" -o $out $optstring;;
				2) cat $right | $wdir/deltaforce -d "$test_delim" -o $out $optstring;;
			esac

			if [ "`diff -q $out $expected`" ]; then
				(( errors++ ))
				echo "failed"
				echo "		see $out"
			else
				echo "passed"
				rm $out
			fi

		else

			echo "creating $expected from $left, $right and options ${tests[$i]}"

			$wdir/mergekeys -d "$test_delim" $optstring $left $right > $expected

		fi

	done

done

echo -e "\n$errors errors\n"
