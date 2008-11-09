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
1;
