BIN_DIR=flowShop_bin

ansible computenodes -a "rm -fr ~/camilo/$BIN_DIR"

for i in `seq 1 5`;
do
    rsync -r $BIN_DIR computenode$i:camilo
done
