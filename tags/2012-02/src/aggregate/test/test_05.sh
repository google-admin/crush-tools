test_number=05
description="labels as field specifiers"

subtest_desc=("default labels" "auto-labels" "user labels")
subtest_opts=(
''
'-L'
'-l "Sum-1	Sum-2	Count-1	Count-2	Average-1	Average-2"'
)

LANG_BAK="$LANG"
LC_NUMERIC_BAK="$LC_NUMERIC"
for subtest in `seq 0 2`; do
  for locale in C en_US.utf8 es_AR.utf8; do
    if [ ! "$(locale -a | grep $locale)" ]; then
      continue
    fi
    export LANG=$locale
    export LC_NUMERIC=$locale
    expected="$test_dir/test_$test_number.$subtest.$locale.expected"
    outfile="$test_dir/test_$test_number.$subtest.$locale.actual"

    eval $bin -p \
         -K Text-1,Text-2 \
         -S Numeric-1,Numeric-2 \
         -C Numeric-1,Numeric-2 \
         -A Numeric-1,Numeric-2 \
         "${subtest_opts[$subtest]}" \
         "$test_dir/test.in" \
         > "$outfile"

    if [ $? -ne 0 ] ||
       [ "`diff -q $outfile $expected`" ]; then
      test_status $test_number $subtest \
                  "$description (${subtest_desc[$subtest]} $locale)" FAIL
    else
      test_status $test_number $subtest \
                  "$description (${subtest_desc[$subtest]} $locale)" PASS
      rm "$outfile"
    fi
  done
done
LANG="$LANG_BAK"
LC_NUMERIC="$LC_NUMERIC_BAK"
