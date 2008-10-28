#!/usr/bin/perl -w

use strict;

die "usage: $0 <args.tab>\n" unless (@ARGV && -f $ARGV[0]);

my (%global, @args);
# this is to get the data structures into the main namespace
# without having to turn off strict
open(ARGFILE, $ARGV[0]) or die "$0: $ARGV[0]: $!\n";
my @argfile_a = <ARGFILE>;
close(ARGFILE);
my $argfile_content = join('', @argfile_a);
eval "$argfile_content";

my $option_list = '';
my ($required_opts, $optional_opts) = ('', '');
foreach my $opt (@args) {
  $opt->{description} =~ s/\\//g;
  if ($opt->{required}) {
    $required_opts .= $opt->{shortopt};
  } else {
    $optional_opts .= $opt->{shortopt};
  }
  if ($opt->{type} eq 'flag' || $opt->{type} eq 'custom_flag') {
    $option_list .= ".TP\n" . '\fB\-' . $opt->{shortopt} .
                    '  \-\-' . $opt->{longopt} . '\fR' .
                    "\n" . $opt->{description} . "\n";
  } else {
    $option_list .= ".TP\n" . '\fB\-' . $opt->{shortopt} .
                    '\fR ' . $opt->{name} .
                    '  \fB \-\-' . $opt->{longopt} . '\fR=' . $opt->{name} .
                    "\n" . $opt->{description} . "\n";
  }
}

if ($required_opts) {
  $required_opts = '<\fI\-' . $required_opts . '\fR>';
}
if ($optional_opts) {
  $optional_opts = '[\fI\-' . $optional_opts . '\fR]';
}

$global{usage_extra} =~ s/\\n\\n/\n.TP\n/g;
$global{usage_extra} =~ s/\\n/ /g;
$global{usage_extra} =~ s/\\t/ /g;

print << "ENDMAN";
.TH $global{name} 1
.SH NAME
.TP
$global{name} \\- $global{description}
.SH SYNOPSIS
.TP
\\fB$global{name}\\fP $required_opts $optional_opts $global{trailing_opts}
.SH DESCRIPTION
$global{description}
$option_list
.TP
$global{usage_extra}
.SH AUTHOR
.TP
Written by the maintainers and contributers of CRUSH tools.  See
http://code.google.com/p/crush-tools
.SH "REPORTING BUGS"
.TP
Report bugs to <crush-tools\@googlegroups.com>, or visit
http://groups.google.com/group/crush-tools
.SH COPYRIGHT
$global{copyright}
ENDMAN
