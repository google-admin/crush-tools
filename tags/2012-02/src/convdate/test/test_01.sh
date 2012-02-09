test_number=01
description="defaults"

expected=$test_dir/test_$test_number.expected
output=$test_dir/test_$test_number.out

cat > $expected << "END_EXPECT"
Date	Field-0	Field-1
2008-10-11-16:32:08	hello	world
2008-10-11-16:32:08	hello	world
END_EXPECT

$bin > $output << "END_TEST"
Date	Field-0	Field-1
10-11-2008-16:32:08	hello	world
10-11-2008-16:32:08	hello	world
END_TEST

if [ $? -ne 0 ] ||
   [ "`diff -q $expected $output`" ]; then
  test_status $test_number 1 "$description" FAIL
else
  test_status $test_number 1 "$description" PASS
  rm $expected $output
fi
