#!/usr/bin/perl -w

use strict;
use Getopt::Std;
use vars qw($opt_h $opt_v $opt_d $opt_f $opt_i $opt_p $opt_b $opt_c);
use Date::Calc qw(Delta_DHMS);
use DART::DateFormat;
use Text::Trim;

# Parse command line options
getopts('hvpd:f:i:b:c:');

# Define default delimiter
my $delim = `tochar 0xfe`;

# Print help if requested
if ($opt_h) {
    print << "HELP";

usage: $0 [-h] [-v] [-p] [-d <delimiter>] [-i <index>] -f <formula> -b <fallback_result> [-c <column_name>]

 -h                       .. help
 -v                       .. verbose (for debugging purposes)
 -p			  .. preserve the header line (just passes the first line through to the output file)
 -d <delimiter>           .. the delimiter for the input file which is read from the standard input; defaults to $delim
 -i <index>               .. index for the new fields (1-based); if not given the new field will be appended
 -f <formula>             .. the formula to calculate from the given fields
 -b <fallback_result>     .. if the formula is not properly evaluated use this result as the fallback
 -c <column_name>         .. the name of the column for the calculated field; only used with option -p

HELP

   exit(0);
}

# Evaluate event ids
my $first = 1;
my $line = "";
my @parts = ();
my $i = 0;
my $result = 0;

# Check delimiter option. Set default if not provided.
if(!defined($opt_d) || $opt_d eq "") {
        $opt_d = $delim;
        print "No delimiter given, assuming $opt_d\n" if(defined($opt_v));
}

# Check index option. Set default if not provided.
if(!defined($opt_i) || !$opt_i =~ m/[1-9][0-9]*/) {
	$opt_i = -1;
        print "No index for the calculated field given, appending the field at the end of the row.\n" if(defined($opt_v));
}

# Check formula. Bail out if it is not given. If given, check the formulas validity.
if(!defined($opt_f)) {
        die "No formula given. Please use option -f to provide one.\n"; exit 1;
} else {
	print "Using formula \"" . $opt_f . "\".\n" if(defined($opt_v));
}

# Check the fallback result. Bail out if it's not given.
if(!defined($opt_b)) {

	# No => But we need one. Bail out.
	die "No fallback result given. Please use option -b to provide one.\n"; exit 1;
}

# Check the column description
if(!defined($opt_c)) {
	$opt_c = "Calculated-Field";
	print "No column name given, using \"$opt_c\" as the column name.\n" if(defined($opt_v));
}

# Read the lines from the standard input
while($line = <STDIN>) {

	# Remove blanks/new lines at the end of the row
	$line = trim $line;

        # Split the line
        @parts = split(/$opt_d/, $line);

        # Pass through the first line which is assumed to be the header
        if($first) {
                # Now we have processed the first row.
                $first = 0;

		# Should we preserve the header?
		if(defined($opt_p)) {
			# Build the new header from the original header and the column name for the calculated field.
			print insert_field($line, \@parts, $opt_d, $opt_c, $opt_i) . "\n";
		}

		# Process the next line.
		next;
        }

	# Calculate the result
	$result = extended_eval($opt_f, \@parts, $opt_b);

	# Build new line with the calculated result
	print insert_field($line, \@parts, $opt_d, $result, $opt_i) . "\n";
}

sub insert_field {
	my ($line, $parts, $delim, $value, $pos) = @_;

        # Put the result into the right place. Should we put this at the beginning or end?
        if($pos == -1 || $pos > scalar @{$parts}) {

                # Yes => Print the line and the given result at the end.
                $line = $line . $delim . $value;
        } elsif($pos == 1) {

                # Yes => Print the result and the line
                $line = $value . $delim . $line;
        } else {

                # No => Put the result in the right place
                my @result = split(/$delim/, $line, $pos);
                my $suffix = pop(@result);

                $line = join($delim, @result) . $delim . $value . $delim . $suffix;
        }

	return $line;
}

sub extended_eval {
	my($formula, $parts, $fallback) = @_;

	my $result = 0, $i = 0;

	$formula = "\$result = " . $formula;
	for($i = 1; $i <= scalar @{$parts}; $i++) {
		$formula =~ s/\[$i\]/$parts->[$i - 1]/g;
	}

	eval($formula);

	if($@) {
		$result = $fallback;
	}

	return $result;
}

