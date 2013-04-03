#!/usr/bin/perl -w

use strict;

my $working_dir = $0;
$working_dir =~ s:/[^/]*$::;
require "$working_dir/utils.pl" || die;

my $curr_test = 0;
my $has_error = 0;
sub chk (@) {
  my ($test, @msg) = @_;
  my $tn = ++$curr_test;
  if ($test) {
    print "PASS: $tn - @msg\n";
  } else {
    print "FAIL: $tn - @msg\n";
    $has_error++;
  }
}

sub arrays_are_equal {
  my ($a, $b) = @_;
  return 0 if (@$a != @$b);
  foreach my $i (0 .. $#{ $a }) {
    return 0 if ($a->[$i] != $b->[$i]);
  }
  return 1;
}

# fields_in_line()
chk fields_in_line('hello,world', ',') == 2,
    "fields_in_line()";
chk fields_in_line('hello', ',') == 1,
    "fields_in_line():single-field line";

# field_str()
chk field_str('hello', "hello,world\n", ',') == 0,
    "field_str():first field";
chk field_str('world', "hello,world\n", ',') == 1,
    "field_str():last field";
chk ! defined field_str('goodbye', "hello,world\n", ','),
    "field_str(): non-existant field";

# get_line_field()
chk get_line_field('1|2|3|4', 0, '|') == 1,
    "get_line_field(): 1st pos" ;
chk get_line_field('1|2|3|4', 3, '|') == 4,
    "get_line_field(): last pos" ;
chk get_line_field('1|2|3|4', 1, '|') == 2,
    "get_line_field(): middle pos" ;

# expand_nums()
my @a;
@a = expand_nums('1-3,5');
chk arrays_are_equal(\@a, [1, 2, 3, 5]),
    "expand_nums(): no adjustment.";
@a = expand_nums('1-3,5', -1);
chk arrays_are_equal(\@a, [0, 1, 2, 4]),
    "expand_nums(): -1 adjustment.";

exit $has_error;
