#!/usr/bin/perl

use strict;
use warnings;

my ( $encname ) = $ARGV[0] =~ m{/([^/.]+).tbl}
   or die "Cannot parse encoding name out of $ARGV[0]\n";

print <<"EOF";
static const struct StaticTableEncoding encoding_$encname = {
  { .decode = &decode_table },
  {
EOF

while( <> ) {
   s/\s*#.*//; # strip comment

   s{^(\d+)/(\d+)}{sprintf "[0x%02x]", $1*16 + $2}e; # Convert 3/1 to [0x31]
   s{"(.)"}{sprintf "0x%04x", ord $1}e;              # Convert "A" to 0x41
   s{U\+}{0x};                                       # Convert U+0041 to 0x0041

   s{$}{,}; # append comma

   print "    $_";
}

print <<"EOF";
  }
};
EOF
