#!/usr/bin/perl -w

use strict;
use Getopt::Long;
use util::args;

my ($help, $keys, $pivot_fields, $vals, $delim, $preserve_headers);
$delim = chr(0xfe);

&GetOptions(	"fields:s"	=> \$keys,
		"pivots:s"	=> \$pivot_fields,
		"values:s"	=> \$vals,
		"delimiter:s"	=> \$delim,
		"keep-headers!"	=> \$preserve_headers,
		"help!"		=> \$help,
	);

if ( $help ) {
	usage();
	exit 1;
}

my (@k, @v, @p);
if ( $keys ) {
	@k = util::args::expand($keys);
}
if ( $vals ) {
	@v = util::args::expand($vals);
}
if ( $pivot_fields ) {
	@p = util::args::expand($pivot_fields);
}

my $line;
my @l;
my $i;
my (@key_headers, @val_headers, @pivot_headers);

if ( $preserve_headers ) {
	$line = <>;
	$line =~ s/[\r\n]//g;
	@l = split(/\Q$delim\E/, $line);
} 

# adjust the indexes from 1-based to 0-based,
# and store the header labels if needed
for ( $i = 0 ; $i <= $#k ; $i++ ) {
	$k[$i]--;
	if ( $preserve_headers ) {
		push @key_headers, $l[$k[$i]];
	}
}

for ( $i = 0 ; $i <= $#v ; $i++ ) {
	$v[$i]--;
	if ( $preserve_headers ) {
		push @val_headers, $l[$v[$i]];
	}
}

for ( $i = 0 ; $i <= $#p ; $i++ ) {
	$p[$i]--;
	if ( $preserve_headers ) {
		push @pivot_headers, $l[$p[$i]];
	}
}

my %data;
my %pivot_names;

while ( $line = <> ) {

	$line =~ s/[\r\n]//g;
	@l = split(/\Q$delim\E/, $line);

	my $lkey = '';
	my $pkey = '';

	for ( $i = 0 ; $i <= $#k ; $i++ ) {
		$lkey .= $l[$k[$i]];
		if ( $i != $#k ) {
			$lkey .= $delim;
		}
	}

	for ( $i = 0 ; $i <= $#p ; $i++ ) {
		$pkey .= $l[$p[$i]];
		if ( $i != $#p ) {
			$pkey .= $delim;
		}
	}

	my @lvals = ();
	if ( defined( $data{$lkey}{ $pkey } ) ) {
		@lvals = @{ $data{$lkey}{ $pkey } };
	}

	for ( $i = 0 ; $i < scalar(@v) ; $i++ ) {
		if ( $l[$v[$i]] !~ /[^\d.]/ ) {
			$lvals[$i] += $l[$v[$i]];
		}
	}

	# make sure we can remember this pivot key's value later
	$pivot_names{ $pkey } = 1;

	$data{ $lkey }{ $pkey } = \@lvals;
}

# sort all the possible pivot names
my @pivots = sort keys %pivot_names;

# precalculate what to print if a given key/pivot combo has no data
my $novalue = "0$delim" x scalar(@v);
$novalue =~ s/\Q$delim\E$//;

# print out the header if required
if ( $preserve_headers ) {
	if ( scalar(@key_headers) ) {
		print join ( $delim, @key_headers ) , $delim;
	}
	for ( $i = 0 ; $i < scalar(@pivots) ; $i++ ) {
		my $pivot_label = $pivots[$i];
		$pivot_label =~ s/\Q$delim\E/ - /g;
		for ( my $j = 0 ; $j < scalar(@val_headers) ; $j++ ) {
			print $pivot_label , ' ' , $val_headers[$j];
			if ( $j != $#val_headers ) {
				print $delim;
			}
		}
		if ( $i != $#pivots ) {
			print $delim;
		}
	}
	print qq(\n);
}

# print out the data
foreach my $lkey ( sort keys %data ) {

	if ( length($lkey) ) {
		print $lkey , $delim;
	}

	for $i ( 0 .. $#pivots ) {
		my $pname = $pivots[$i];
		if ( defined( $data{$lkey}{$pname} ) ) {
			for my $j ( 0 .. $#v ) {
				print $data{$lkey}{$pname}->[$j] || 0;
				if ( $j != $#v ) {
					print $delim;
				}
			}
		} else {
			print $novalue;
		}
		if ( $i != $#pivots ) {
			print $delim;
		}
	}
	print qq(\n);
}

exit(0);


sub usage {
	print STDERR <<ENDUSAGE;

usage: $0 <-f <fields>> <-p <pivot-flds>> <-v <value-flds>> [-d <delim>] [-k]

	-f	these fields act as the keys for the new rows
	-p	these field values become new columns
	-v	these field values are summed for each of the pivot fields
	-k	preserves the header line
	-d	default is 0xfe

ENDUSAGE
}
