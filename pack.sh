#!/bin/bash

DIR_TO_PACK=flowShopEo
BUILD_DIR=$DIR_TO_PACK"/build"
OUTPUT_DIR=flowShop_bin

# prev file
rm -rf $OUTPUT_DIR.tgz
mkdir $OUTPUT_DIR

# Create structure
mkdir $OUTPUT_DIR/application

# Executable
cp $BUILD_DIR/application/FlowShop* $OUTPUT_DIR/application/
# Copy all resources on package
cp -r package/* $OUTPUT_DIR/
# Make sure host is inside app for easy reference
mv $OUTPUT_DIR/hosts $OUTPUT_DIR/application/
# Benchmark files
cp -r $DIR_TO_PACK/application/instances $OUTPUT_DIR/application/
# Make sure to copy parameters
cp $DIR_TO_PACK/application/*.param $OUTPUT_DIR/application/
# Libs
cp -r $BUILD_DIR/lib $OUTPUT_DIR/

tar cvzf $OUTPUT_DIR.tgz $OUTPUT_DIR
# clean up
rm -rf $OUTPUT_DIR
