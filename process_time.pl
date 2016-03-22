use strict;
use warnings;

# Read fitness
my $rfile = $ARGV[0];
my $wfile = $ARGV[1];
open(my $rfh, "<", $rfile)
    or die "Cannot open $rfile: $!";
open(my $wfh, ">", $wfile . '.out')
    or die "Cannot write $wfile: $!";
my @rows = ();
my $row = [];
while (my $line = <$rfh>) {
    if ($line =~ /^Benchmark/) {
        print $wfh join("\t",@$row)."\n" if scalar @$row;
        $row = [];
    } elsif ($line =~ /^real/) {
        if($line =~ /(\d+)m(\d+\.\d+)s$/) {
            my $seconds = $2; # Second match minutes
            $seconds += 60 * $1; # First group minutes
            push $row, $seconds;
        } else {
            push $row, "error";
        }
    }
}
print $wfh join("\t",@$row)."\n";
close $rfh;
close $wfh;