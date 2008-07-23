#!/usr/bin/perl -w

use strict;

my $args_tab = $ARGV[0];

our (%global, @args);

require $args_tab || die "$!";
$global{trailing_opts} = '' unless $global{trailing_opts};
$global{usage_extra} = '' unless $global{usage_extra};
$global{usage_extra} =~ s/\\n/\n/g;

print "#summary $global{name} - $global{description}\n\n",
      "= Usage =\n\n",
      "`$global{name} <options> $global{trailing_opts}`\n\n",
      "$global{usage_extra}\n\n",
      "= Options =\n",
      "||*Short Opt*||*Long Opt*||*Argument*||*Description*||\n";

foreach my $arg (@args) {
  if ($arg->{shortopt}) {
    $arg->{shortopt} = '-' . $arg->{shortopt};
  } else {
    $arg->{shortopt} = ' ';
  }
  if ($arg->{longopt}) {
    $arg->{longopt} = '--' . $arg->{longopt};
  } else {
    $arg->{longopt} = ' ';
  }
  print "||$arg->{shortopt}||$arg->{longopt}||";
  if ($arg->{type} =~ /var/) {
    print "<$arg->{name}>";
  }
  print " ||$arg->{description}||\n";
}

print "\n----\n",
      "back to UserDocs\n";

exit(0);
