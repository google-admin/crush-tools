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
echo -n '' | ./truncfield -f 1 => ''
# stdin
cat test.in0 test.in1 | ./truncfield -f 1,3 -d , | tr '\\n' ' ' | trim => ,02,,04,05 ,12,,14,15
./truncfield -f 2,4 -d , < test.in0 => 01,,03,,05
# one and two trailing input files, using env variable for delimiter
DELIMITER=',' ./truncfield -f 3-5 test.in0 => 01,02,,,
./truncfield -f 3-5 -d , test.in0 test.in1 | tr '\\n' ' ' | trim => 01,02,,, 11,12,,,
# valid input field boundaries
./truncfield -f 1,5 -d , test.in0 => ,02,03,04,
# test the default delimiter
./truncfield -f 2-4 test.in2 => 21þþþþ25
# invalid params
./truncfield -f 6-10 -d , test.in0 => 01,02,03,04,05
./truncfield -f 0 -d , test.in0 => 01,02,03,04,05
ENDTESTS

rm test.*

exit 0

