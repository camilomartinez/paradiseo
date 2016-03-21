#!/bin/bash

EXP_DIR=flowShop_bin_exp_1
OUTPUT_DIR=$EXP_DIR/processed

# prev dir
rm -rf $OUTPUT_DIR
mkdir $OUTPUT_DIR

# filter fitness
perl process_fitness.pl $EXP_DIR'/application/log_fitness.txt' $OUTPUT_DIR'/fitness_only.txt'
