#!/usr/bin/perl -w

use strict;

my $working_dir = $0;
$working_dir =~ s:/[^/]*$::;
require "$working_dir/date_parse_format.pl" || die;

my $cur_test = 0;
my $failures = 0;

test_parse("YYYY-MM-DD",
           "2008-09-06",
           ['2008', '09', '06', '0', '0', '0']);

test_parse("YYYY-MM-DD HH:MI:SS",
           "2008-09-06 10:20:30",
           ['2008', '09', '06', '10', '20', '30']);

test_format("YYYY-MM-DD",
            ['2008', '09', '06', '0', '0', '0'],
            "2008-09-06");

test_format("YYYY-MM-DD HH:MI:SS",
            ['2008', '09', '06', '10', '20', '30'],
            "2008-09-06 10:20:30");

if ($failures) {
  exit(1);
}
exit(0);



sub test_parse {
  my $fmt = shift;
  my $date = shift;
  my $expect = shift;

  $cur_test++;
  my @result = date_parse($fmt, $date);
  if (assert_equal(\@result, $expect)) {
    print "PASS: $cur_test - date_parse()\n";
  } else {
    print "FAIL: $cur_test - date_parse(): ",
          "(", join(',', @result), ") instead of (",
          join(',', @{$expect}), ")\n";
    $failures++;
  }
}


sub test_format {
  my $fmt = shift;
  my $date = shift;
  my $expect = shift;

  $cur_test++;
  my $result = date_format($fmt, @{$date});
  if (assert_equal($result, $expect)) {
    print "PASS: $cur_test - date_format()\n";
  } else {
    print "FAIL: $cur_test - date_format(): ",
          "\"$result\" instead of \"$expect\".\n";
    $failures++;
  }
}


sub assert_equal {
  my $a = shift;
  my $b = shift;

  if (ref($a) eq 'ARRAY' && ref($b) eq 'ARRAY') {
    my $a_len = scalar(@{$a});
    my $b_len = scalar(@{$b});
    if ($a_len != $b_len) {
      return 0;
    }
    for my $i ( 0 .. $a_len - 1 ) {
      if ($a->[$i] ne $b->[$i]) {
        return 0;
      }
    }
  } else {
    if ($a ne $b) {
      return 0;
    }
  }

  return 1;
}
