#!/bin/bash

DIR_TO_PACK=flowShopEo
BUILD_DIR=$DIR_TO_PACK"/build"
OUTPUT_DIR=$DIR_TO_PACK"_bin"

# prev file
rm -rf $OUTPUT_DIR.tgz
mkdir $OUTPUT_DIR

# Create structure
mkdir $OUTPUT_DIR/application

# Executable
cp $BUILD_DIR/application/FlowShop* $OUTPUT_DIR/application/
# Run script removing build dir
cp $DIR_TO_PACK/*Run.sh $OUTPUT_DIR/
sed -i 's@build/application@application@g' $OUTPUT_DIR/*Run.sh
# Benchmark files
cp -r $DIR_TO_PACK/application/instances $OUTPUT_DIR/application/
# Make sure to copy parameters
cp $DIR_TO_PACK/application/*.param $OUTPUT_DIR/application/
# Libs
cp -r $BUILD_DIR/lib $OUTPUT_DIR/

tar cvzf $OUTPUT_DIR.tgz $OUTPUT_DIR
# clean up
rm -rf $OUTPUT_DIR
