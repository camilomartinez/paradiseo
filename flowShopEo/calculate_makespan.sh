#!/usr/bin/env bash

perl calculate_makespan.pl application/instances/ta002.txt "5 9 16 6 17 13 14 2 11 8 7 19 1 12 4 18 10 0 3 15"
printf "\n"
perl calculate_makespan.pl application/instances/ta002.txt "5 9 16 6 18 19 11 0 15 2 8 14 13 12 3 17 10 4 7 1"
printf "\n"
perl calculate_makespan.pl application/instances/ta002.txt "5 9 16 6 18 19 14 8 11 12 7 2 4 13 15 0 17 3 10 1"

