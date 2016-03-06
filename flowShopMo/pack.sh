#!/bin/bash

BUILD_DIR=build
OUTPUT_DIR=flowShop_bin

# prev file
rm -rf $OUTPUT_DIR.tgz
mkdir $OUTPUT_DIR

# Create structure
mkdir $OUTPUT_DIR/application

# Executable
cp $BUILD_DIR/application/FlowShop* $OUTPUT_DIR/application/
# Benchmark files
cp -r $BUILD_DIR/application/instances $OUTPUT_DIR/application/
# Libs
cp -r $BUILD_DIR/lib $OUTPUT_DIR/

tar cvzf $OUTPUT_DIR.tgz $OUTPUT_DIR
# clean up
rm -rf $OUTPUT_DIR
