use strict;
use warnings;
use Data::Dumper;
use List::Util qw(max);

# Check arguments
my $num_args = scalar @ARGV;
if ($num_args < 2) {
    die "\nUsage: calculate_makespan benchmarkFile scheduling";
}

#Read processing times
my $filename = $ARGV[0];
my @processing = ();
open(my $fh, "<", $ARGV[0])
    or die "Cannot open $filename: $!";

my $line = readline($fh);
chomp $line;
my @fields = split "\\s", $line;
# Number of jobs
my $n = $fields[0];
# Number of machines
my $m = $fields[1];
while ($line = <$fh>) {
    chomp $line;
    my @times = split "\\s+", $line;
    my $machine_times = [];
    for my $i (0 .. $n-1)
    {
        push $machine_times, $times[$i];
    }
    push @processing, $machine_times;
}
#print Dumper \@processing;

# Read scheduling
my $scheduling_input = $ARGV[1];
my @scheduling = split "\\s", $scheduling_input;

# Initialize completion times
my @completion = ();
for my $i (0 .. $m-1) {
    my @zeros = map { 0 } 1..$n;
    push @completion, \@zeros;
}

#print Dumper \@completion;

# Completion first job
my $first_job = $scheduling[0];
$completion[0][$first_job] = $processing[0][$first_job];
for my $i (1 .. $m-1) {
    $completion[$i][$first_job] = $completion[$i-1][$first_job] + $processing[$i][$first_job];
}
#print Dumper \@completion;

# Completion first machine
for my $j (1 .. $n-1) {
    my $prev = $scheduling[$j-1];
    my $curr = $scheduling[$j];
    $completion[0][$curr] = $completion[0][$prev] + $processing[0][$curr];
}
#print Dumper \@completion;

# Completion all machines
for my $i (1 .. $m-1) {
    for my $j (1 .. $n-1) {
        my $prev = $scheduling[$j-1];
        my $curr = $scheduling[$j];
        # Cannot start till the prev job finishes in this machine
        # and this job finishes in the prev machine
        $completion[$i][$curr] = max($completion[$i][$prev], $completion[$i-1][$curr]) + $processing[$i][$curr];
    }
}
#print Dumper \@completion;

print "Scheduling: ", join(" ", @scheduling), "\n";
print "Makespan: ", $completion[$m-1][$scheduling[$n-1]], "\tCompletion times for jobs are:\n";
for my $j (0 .. $n-1) {
    print $completion[$m-1][$scheduling[$j]], " ";
}
print "\n";