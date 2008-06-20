#!/bin/bash

# generate some test input files
d=`tochar 0xfe`
echo "01,02,03,04,05" > test.in0
echo "11,12,13,14,15" > test.in1
echo "21${d}22${d}23${d}24${d}25" > test.in2

# checks for the truth of a test & prints feedback
# param: test label
# param: statement to test
function assert {
	local n=$1
	shift
	echo -n "test $n: "
	eval "test $* || { echo \"failed ($*)\"; return 1; }"
	echo "ok"
	return 0
}

# --- check input methods - stdin and trailing args ---

declare -i n
n=1

while [ true ]; do

	read line || break

	if [ "${line:0:1}" == '#' ]; then
		continue
	fi

	command=`echo "$line" | sed 's/ => .*//'`
	expected=`echo "$line" | sed 's/.* => //'`

	if [ ! "$command" ]; then
		continue
	fi

	result=$( $SHELL -c "$command" )
	assert $n "'$result' == '$expected'"
	(( n++ ))

done <<"ENDTESTS"
#
# format: [command] => [expected result]
#
# no input
echo -n '' | ./reorder -f 1 => ''
# stdin
cat test.in0 test.in1 | ./reorder -m 2,3 -d , | tr '\\n' ' ' | trim => 01,03,02,04,05 11,13,12,14,15
./reorder -s 1,2 -d , < test.in0 => 02,01,03,04,05
# one and two trailing input files
./reorder -f 1,3-5 -d , test.in0 => 01,03,04,05
./reorder -f 3-5 -d , test.in0 test.in1 | tr '\\n' ' ' | trim => 03,04,05 13,14,15
# valid input field boundaries
./reorder -s 1,5 -d , test.in0 => 05,02,03,04,01
./reorder -m 1,5 -d , test.in0 => 02,03,04,05,01
./reorder -m 5,1 -d , test.in0 => 05,01,02,03,04
./reorder -f 5,1 -d , test.in0 => 05,01
# test the default delimiter
./reorder -f 1,5 test.in2 => 21þ25
# invalid params
./reorder -f 3-10 -d , test.in0 => 03,04,05,,,,,
./reorder -f 0 -d , test.in0 => 01
# these two cause reorder to emit errors (suppressed here) and exit
./reorder -m 1 -d , test.in0 2> /dev/null => ''
./reorder -s 1 -d , test.in0 2> /dev/null => ''
# swap and move are broken when accessing out-of-bounds fields, so the
# following two tests fail.
# ./reorder -s 3,10 -d , test.in0 => 01,02,,04,05,,,,,03
# ./reorder -m 3,10 -d , test.in0 => 01,02,04,05,,,,,03
ENDTESTS

rm test.*

exit 0

