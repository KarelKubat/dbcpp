#!/usr/bin/env perl

use strict;

# Need file arg
die("Usage: loadlist.pl FILENAME\n") if ($#ARGV != 0);

# Open & load
open(my $if, $ARGV[0]) or die("Cannot read $ARGV[0]: $!\n");
while (my $line = <$if>) {
  chomp($line);
  my ($rank, $title, $artist, $year) = split(/\t/, $line);
  die("Bad line '$line' (not 4 fields)\n") if ($year eq '');
  system("./top2000 add $rank \"$title\" \"$artist\" $year")
    and die("Failed to add to db\n");
}
