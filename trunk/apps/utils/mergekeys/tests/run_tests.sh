#!/bin/bash

tests=(00 01 02 03 04 05)
test_delim='\t'
wdir=`dirname $0`

for t in ${tests[*]}; do

	echo -n "test $t: "

	$wdir/../mergekeys -d "$test_delim" \
			$wdir/test_${t}.a $wdir/test_${t}.b \
			> $wdir/test_${t}.out

	if [ "`diff -q $wdir/test_${t}.out $wdir/test_${t}.expected`" ]; then
		echo "failed (see $wdir/test_${t}.out)"
	else
		echo "passed"
		rm $wdir/test_${t}.out
	fi
done

