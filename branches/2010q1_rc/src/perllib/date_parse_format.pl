
sub date_parse {
  my $fmt = shift;
  my $datestr = shift;

  return undef unless ($fmt && $datestr);

  my $YY  = index($fmt, 'YYYY');
  my $MM  = index($fmt, 'MM');
  my $DD  = index($fmt, 'DD');
  my $HH  = index($fmt, 'HH');
  my $MI  = index($fmt, 'MI');
  my $SS  = index($fmt, 'SS');

  my ($y, $m, $d, $h, $i, $s) = (0, 0, 0, 0, 0, 0);
  $y = substr($datestr, $YY, 4) if($YY != -1);
  $m = substr($datestr, $MM, 2) if($MM != -1);
  $d = substr($datestr, $DD, 2) if($DD != -1);
  $h = substr($datestr, $HH, 2) if($HH != -1);
  $i = substr($datestr, $MI, 2) if($MI != -1);
  $s = substr($datestr, $SS, 2) if($SS != -1);
  
  return ($y, $m, $d, $h, $i, $s);
}

sub date_format {
  my $fmt = shift;
  my ($y, $m, $d, $h, $i, $s) = @_;

  return undef unless ($fmt);

  $y = sprintf("%.4d", $y) if defined($y);
  $m = sprintf("%.2d", $m) if defined($m);
  $d = sprintf("%.2d", $d) if defined($d);
  $h = sprintf("%.2d", $h) if defined($h);
  $i = sprintf("%.2d", $i) if defined($i);
  $s = sprintf("%.2d", $s) if defined($s);

  $fmt =~ s/YYYY/$y/g;
  $fmt =~ s/MM/$m/g;
  $fmt =~ s/DD/$d/g;
  $fmt =~ s/HH/$h/g;
  $fmt =~ s/MI/$i/g;
  $fmt =~ s/SS/$s/g;

  return $fmt;
}

1;
