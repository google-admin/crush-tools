=item * expand_chars

expand escape sequences like '\t' in a string to their expansions.

=cut
sub expand_chars {
  my $d = shift || return;
  eval("sprintf(\"$d\")");
}

=item * field_str()

returns the 0-based index of the first field in a delimited string equal to
the specified value, or undef if not found.

=cut
sub field_str {
  my $value = shift;
  my $string = shift;
  my $delim = shift;
  $string =~ s/[\r\n]//g;
  my @a = split(/\Q$delim\E/, $string);
  my $i;
  for $i (0 .. $#a) {
    if ($a[$i] eq $value) {
      return $i;
    }
  }
  return undef;
}

=item * fields_in_line()

Counts the number of fields in a delimited string.

=cut
sub fields_in_line {
  my $str = shift;
  my $delim = shift;
  my $n = 1;
  my $i = 0;
  while (($i = index($str, $delim, $i)) > 0) {
    $n++;
    $i += length($delim);
  }
  return $n;
}

=item * get_line_field($line, $field_index, $delim)

Get the data at position field from the delim deliminated string line.

$field_index is 0 based

=cut
sub get_line_field {
  my $pos = 0;
  for (my $i = 0; $i < $_[1]; $i++) {
    $pos = index($_[0], $_[2], $pos);
    $pos++;
  }
  my $end_pos = index($_[0], $_[2], $pos) - $pos;
  $end_pos = length($_[0]) - $pos if $end_pos <= 0;
  return substr($_[0], $pos, $end_pos);
}

=item * expand_nums($arg [, $adjust])

Turn a string of comma-separated numbers and number ranges into an array of
numbers. If specified, $adjust is added to each value after expansion. E.g.,
If turning 1-based field indexes into array indexes, pass -1 as the adjust
value.

=cut
sub expand_nums {
  my $arg = shift;
  my $adjust = shift || 0;
  my @fields = split(',', $arg);
  my @idxs = ();
  foreach my $f (@fields) {
    if ($f =~ /(\d+)-(\d+)/) {
      push(@idxs, $1 .. $2);
    } elsif ($f =~ /\d+/) {
      push(@idxs, $f);
    } else {
      use Carp;
      croak "Invalid value in numeric list: $f";
    }
  }
  if ($adjust) {
    foreach my $i (0 .. $#idxs) {
      $idxs[$i] += $adjust;
    }
  }
  return @idxs;
}


1;
