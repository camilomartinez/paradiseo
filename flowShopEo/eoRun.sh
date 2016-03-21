#!/bin/bash

(cd build/application && \
 rm -rf Res && \
 rm -f log_* && \
 time mpirun -np 6 FlowShopEO @FlowShopEO.param -B=instances/ta001.txt)
