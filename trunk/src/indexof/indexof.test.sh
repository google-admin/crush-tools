#!/bin/bash

# generate some test input files
export DELIM=`tochar 0xfe`
echo "Time${DELIM}User-ID${DELIM}Ad-ID" > test.in0
(echo ""; echo "Time${DELIM}User-ID${DELIM}Ad-ID") > test.in1

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
cat test.in0 | ./indexof -l "User-ID" -d "$DELIM" => 2
cat test.in1 | ./indexof -r 2 -l "User-ID" -d "$DELIM" => 2
./indexof -f test.in0 -l "User-ID" -d "$DELIM" => 2
./indexof -f test.in1 -r 2 -l "User-ID" -d "$DELIM" => 2
./indexof -s "`cat test.in0`" -l "User-ID" -d "$DELIM" => 2
./indexof -f test.in0 -l "Advertiser-ID" -d "$DELIM" => 0
# non-existant input file
./indexof -f test.in -l "User-ID" -d "$DELIM" => ''
ENDTESTS

rm test.in*

exit 0

