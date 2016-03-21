#!/bin/bash

APP=build/application
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
        # Execute
        (cd $APP && \
         { time mpirun -np 5 $BINARY @"$PARAM" $benchmarkFile >> $RESULTS ; } 2>> $TIME )
        echo 'Finished run #'$run
    done
done