#!/usr/bin/perl -w

use strict;

my $working_dir = $0;
$working_dir =~ s:/[^/]*$::;
require "$working_dir/utils.pl" || die;

my $has_error = 0;

if (fields_in_line('hello,world', ',') == 2) {
  print "PASS: fields_in_line()\n";
} else {
  print "FAIL: fields_in_line()\n";
  $has_error = 1;
}
if (fields_in_line('hello', ',') == 1) {
  print "PASS: fields_in_line() on single-field line\n";
} else {
  print "FAIL: fields_in_line() on single-field line\n";
  $has_error = 1;
}


if (field_str('hello', "hello,world\n", ',') == 0) {
  print "PASS: field_str() on first field\n";
} else {
  print "FAIL: field_str() on first field\n";
  $has_error = 1;
}
if (field_str('world', "hello,world\n", ',') == 1) {
  print "PASS: field_str() on last field\n";
} else {
  print "FAIL: field_str() on last field\n";
  $has_error = 1;
}
if (! defined field_str('goodbye', "hello,world\n", ',')) {
  print "PASS: field_str() on non-existant field\n";
} else {
  print "FAIL: field_str() on non-existant field\n";
  $has_error = 1;
}

exit $has_error;
