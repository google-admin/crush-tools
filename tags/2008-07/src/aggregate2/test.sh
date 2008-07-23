#!/bin/bash

wdir=`dirname $0`

datadir=$wdir/test
infile=$datadir/test.in

tests=("-p -k 1 -s 3" "-p -k 1,2 -s 3,4" "-p -k 1 -s 5" "-p -k 1,2 -s 5")
test_names=(	"one key, one sum"
		"multi-key, multi-sum"
		"one key, float sum"
		"multi-key, float sum"
	)
errors=0

maxlen=0
for i in `seq 0 $(( ${#test_names[*]} - 1 ))`; do
	curlen=`echo -n "${test_names[$i]}" | wc -m`
	if [ $curlen \> $maxlen ]; then maxlen=$curlen; fi
done

echo "" # just put an empty line for readability

export DELIMITER='\t'

for i in `seq 0 $(( ${#tests[*]} - 1 ))`; do
	optstring=`echo ${tests[$i]} | sed 's/\s//g'`
	
	curlen=`echo -n "${test_names[$i]}" | wc -m`
	printf "test %.2d - %s :%*s\t" $i "${test_names[$i]}" $(( $maxlen - $curlen )) " "

	if [ -e $infile.$optstring.expected ]; then

		actual=$infile.$optstring.actual
		expected=$infile.$optstring.expected

		${wdir}/aggregate2 ${tests[$i]} $infile \
			> $actual

		if [ "`diff -q $actual $expected`" ]; then
			(( errors++ ))
			echo "failed"
			echo "		see $actual"
		else
			echo "passed"
			rm $actual
		fi
  else
    echo "skipped"
			echo "		$infile.$optstring.expected does not exist"
	fi
done

echo -e "\n$errors errors\n"

