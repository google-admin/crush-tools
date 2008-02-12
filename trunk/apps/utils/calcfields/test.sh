#!/bin/bash

wdir=`dirname $0`

tests=(00 01 02)

test_opts=(	"-d ';' -e '[3] / [2]' -b '0.0'" 
		"-p -d ';' -e '[Clicks] / [Impressions]' -b '0.0'"
		"-p -e 'sprintf \"%.3f\", [Clicks]/[Impressions] * 100' -b 'N/A'"
	  )

test_inputs=(	"$wdir/test/test.txt"
		"$wdir/test/test.txt"
		"$wdir/test/imps_clks.log"
	    )

for i in `seq 0 $(( ${#tests[*]} - 1 ))`; do
	echo -n "test ${tests[$i]}: "

	expected_out=$wdir/test/test_${tests[$i]}.expected
	actual_out=$wdir/test/test_${tests[$i]}.out

	eval "$wdir/calcfields.pl ${test_opts[$i]} < ${test_inputs[$i]} > $actual_out"

	has_diff=`diff -q $expected_out $actual_out`
	if [ $? -eq 2 ]; then
		echo "diff failed"
	elif [ "$has_diff" ]; then
		echo "failed - see $actual_out"
	else
		echo "passed"
		rm $actual_out
	fi
done
