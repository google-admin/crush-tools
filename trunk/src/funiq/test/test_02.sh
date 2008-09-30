test_number=02
description="middle field without dups"

expected=$test_dir/test_$test_number.expected
output=$test_dir/test_$test_number.out

cat > $expected << "END_EXPECT"
f0	f1	f2
00	001	002
00	002	004
00	003	008
10	111	112
10	112	114
10	113	118
20	221	222
20	222	224
20	223	228
END_EXPECT

$bin -f 2 > $output << "END_INPUT"
f0	f1	f2
00	001	002
00	002	004
00	003	008
10	111	112
10	112	114
10	113	118
20	221	222
20	222	224
20	223	228
END_INPUT

if [ $? -ne 0 ] || [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $expected $output
fi
