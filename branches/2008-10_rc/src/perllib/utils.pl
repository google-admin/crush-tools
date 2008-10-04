=item * expand_chars

expand escape sequences like '\t' in a string to their expansions. 

=cut
sub expand_chars {
  my $d = shift || return;
  eval("sprintf(\"$d\")");
}

1;
