#!/usr/bin/perl -w

use strict;
use DBI;

# DBM supports only 2 columns unless the MLDBM package is installed.
# We'll just go with what can be expected to be on the system.

my @seed = (['Anathem', 935],
            ['APUE 2e', 960]);

my $dbh = DBI->connect($ENV{DB_DSN});
my $qh = $dbh->prepare("CREATE TABLE crush_test
                        (Title TEXT, Pages INTEGER)");
$qh->execute();

$qh = $dbh->prepare("INSERT INTO crush_test (Title, Pages)
                                     VALUES (?, ?)");
foreach my $row (@seed) {
  $qh->execute(@{$row});
}
$dbh->disconnect();
exit(0);
