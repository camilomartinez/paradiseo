#!/bin/bash

# Update binaries   "distribute.sh"
# Run               "$(nohup bash experiment.sh &> log_experiment.txt)"
# Verify            "ps -fu ubuntu"
# Force end         "pkill -f experiment1.sh"

APP=application
RESULTS=log_fitness.txt
TIME=log_time.txt
BINARY=FlowShopEO
PARAM=$BINARY'.param'

#Cleanup
(cd $APP && \
 rm -rf Res && \
 rm -f log_*)

declare -a instances=('1 23 54');
#declare -a instances=$(seq 1 120);

echo 'Start experiment'
for instance in ${instances[@]}; do
    # Pad instance number
    if [ $instance -lt 10 ]; then
        instance="00"$instance
    elif [ $instance -lt 100 ]; then
        instance="0"$instance
    fi

    # Replace with new instance
    benchmark='Benchmark '$instance
    echo "$benchmark" | tee -a $APP/$RESULTS $APP/$TIME
    benchmarkFile="-B=instances/"'ta'$instance'.txt'
    for run in $(seq 1 5); do
        # Running on 6 nodes
        # 1 master and 5 compute, 8 core each.
        (cd $APP && \
         { time mpirun --hostfile hosts -np 21 --bynode --nooversubscribe $BINARY @"$PARAM" $benchmarkFile >> $RESULTS ; } 2>> $TIME )
        echo 'Finished run #'$run
    done
done
echo 'Finished experiment'
