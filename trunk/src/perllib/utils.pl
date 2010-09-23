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

1;
