#!/usr/bin/perl -w

use strict;
use FindBin ();

$ENV{FIELDSPLIT_FLUSH} = 2;

my $verbose = 0;

my $wdir = $FindBin::Bin;
my $exe  = "$wdir/fieldsplit";

my %tests = (
	'001' => { description => "preserve order when performing user-supplied transformations.",
		   params => [
			'-f', '1',
			'-s', '.actual',
			'-x', '\'s/^(\d+)-(\d+)-\d+/$1-$2/\'',
			'-d', "\"\t\"",
			'-p', "$wdir/test/001",
			"$wdir/test/001-transform-key.txt"
			],
		   expected_return => 0,
		 },
	'002' => { description => "fail when missing required params.",
		   params => [],
		   expected_return => 1,
		   discard_output => 1,
		 },
);

my $n_failed = 0;

foreach my $test ( sort keys %tests ) {

	print "running $exe @{ $tests{$test}->{params} }\n" if $verbose;

	print "test $test:\t";

	# build param string.
	# just calling system($exe ...) doesn't allow discarding output
	my $params = join ' ', @{ $tests{$test}->{params} };
	if ( $tests{$test}->{discard_output} ) {
		$params .= " >/dev/null 2>&1";
	}

	my $retval = system($ENV{SHELL}, "-c", "$exe $params" );
	$retval &= 0xFFFF;

	if ( $retval == 0xFF00 ) {
		warn "execution failed: $!\n";
		next;
	}

	$retval /= 256;
	if ( $retval != $tests{$test}->{expected_return} ) {
		print "failed (returned $retval instead of $tests{$test}->{expected_return})\n";
		$n_failed++;
		next;
	}

	opendir( DIR, "$wdir/test/$test" ) or die "$wdir/test/$test: $!\n";
	my @dir_contents = sort readdir DIR;
	closedir(DIR);

	my ( %expected_output, %actual_output );
	foreach my $file ( grep(/expected$/, @dir_contents) ) {
		$file =~ s/.expected//;
		$expected_output{ $file } = 1;
	}
	foreach my $file ( grep(/actual$/, @dir_contents) ) {
		$file =~ s/.actual//;
		$actual_output{ $file } = 1;
	}

	if ( $verbose ) {
		print "\nexpected: ", (join ' ', keys %expected_output) , "\n";
		print "actual:   ",   (join ' ', keys %actual_output)   , "\n";
	}

	if ( set_diff( \%expected_output, \%actual_output )
	  || set_diff( \%actual_output, \%expected_output ) ) {
		print "failed (mismatched output files)\n";
		$n_failed++;
		next;
	}
	
	for my $e_fname ( keys %expected_output ) {
		my $a_fname = $e_fname;
		$e_fname .= '.expected';
		$a_fname .= '.actual';
		my $is_different = `diff -q $wdir/test/$test/$e_fname $wdir/test/$test/$a_fname`;
		if ( $is_different ) {
			print "failed ($e_fname differs from $a_fname)\n";
			$n_failed++;
			next;
		}
	}

	print "passed\n";
	foreach my $tmpfile ( keys %actual_output ) {
		print "unlinking $tmpfile.actual\n" if $verbose;
		unlink "$wdir/test/$test/$tmpfile.actual"
			or warn "unlink $wdir/test/$test/$tmpfile.actual: $!\n";
	}
}

exit( $n_failed );

####

# return items in set A that are not members of set B
# params are hashrefs with set members as keys
sub set_diff {
	my $a = shift;
	my $b = shift;
	my @diff = ();
	foreach my $m ( keys %{ $a } ) {
		push(@diff, $m) unless ( defined( $b->{$m} ) );
	}
	return @diff;
}

