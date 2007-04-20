#!/usr/bin/perl -w -I/usr/local/devprac/lib/pm

use strict;
use DART::FileChecker;

my $d = 0;

if(scalar(@ARGV) == 0 || $ARGV[0] eq '-h'){
	print STDERR "usage: $0 <logfile> <directory> <regex> [-d]\n";
	print STDERR "\nthe last option specifies that the directory\n" .
		"should be included in the output.\n";
	exit(1);
}

my $donelog = $ARGV[0];
my $fdir = $ARGV[1];
my $fpattern = $ARGV[2];
if(defined($ARGV[3]) && $ARGV[3] eq '-d'){
	$d = 1;
}
my @files_to_run = DART::FileChecker->dir_file_diff($donelog, $fdir, $fpattern);

foreach my $f(@files_to_run){
	print $d ? $fdir : "" , $f , q( );
}

exit(0);
