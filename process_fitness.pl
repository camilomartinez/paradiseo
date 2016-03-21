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
    } else {
        chomp $line;
        push $row, $line;
    }
}
print $wfh join("\t",@$row)."\n";
close $rfh;
close $wfh;