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

This is perf optimized for fields that occur in the first N positions
of the line, where N is some low number. The alorithm builds an array
for each field in the line and returns the one at the requested
index. So the smaller the array, the faster the function.

$field_index is 0 based

=cut    
sub get_line_field {
  # the field index increment is 1 for 0-based index + 1 for next pos
  return (split(/\Q$_[2]\E/o, $_[0], $_[1] + 2))[$_[1]];
}

1;
