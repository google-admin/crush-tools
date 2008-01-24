#!/bin/bash

wdir=`dirname $0`

datadir=$wdir/tests
test_delim='\t'

tests=(00 01 02)
test_opts=( "" "" "-k 2" )
test_names=( "defaults" "defaults" "key field" )

subtests=("files for both" "stdin on left" "stdin on right")
files=(
	"\$left \$right"
	"- \$right"
	"\$left -"
)

errors=0

for i in `seq 0 $(( ${#tests[*]} - 1 ))`; do

	echo "" # just put an empty line for readability

	left=$datadir/test_${tests[$i]}-full.txt
	right=$datadir/test_${tests[$i]}-delta.txt

	# output should be the same for all 3 ways of passing input
	expected=$datadir/test_${tests[$i]}.expected

	for st in `seq 0 $(( ${#subtests[*]} - 1 ))`; do
		out=$datadir/test_${tests[$i]}_subtest_$st.out

		eval "fileopts=\"${files[$st]}\""

		if [ -e $expected ]; then

			echo -ne "test $i ${test_names[$i]} (${subtests[$st]}):\t"

			case $st in
				0) $wdir/deltaforce -d "$test_delim" ${test_opts[$i]} -o $out $fileopts;;
				1) cat $left | $wdir/deltaforce -d "$test_delim" ${test_opts[$i]} -o $out $fileopts;;
				2) cat $right | $wdir/deltaforce -d "$test_delim" ${test_opts[$i]} -o $out $fileopts;;
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
			echo "no expected output for test $i" >&2
		fi

	done

done

echo -e "\n$errors errors\n"

exit $errors
