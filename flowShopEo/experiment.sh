#!/bin/bash

APP=build/application
RESULTS=log_fitness.txt
TIME=log_time.txt

#Cleanup
(cd $APP && \
 rm -rf Res && \
 rm -f log_*)

declare -a instances=('001 054');

echo 'Start experiment'
for instance in "${instances[@]}"; do
    new='ta'$instance'.txt'
    # Replace with new instance
    sed -i -r "s/ta([0-9]{3})\.txt/$new/g" $APP'/FlowShopEO.param'
    benchmark='Benchmark '$instance
    echo "$benchmark" | tee -a $APP/$RESULTS $APP/$TIME
    for run in $(seq 1 5); do
        # Execute
        (cd $APP && \
         { time mpirun -np 5 FlowShopEO @FlowShopEO.param >> $RESULTS ; } 2>> $TIME )
        echo 'Finished run #'$run
    done
done