#!/bin/bash


(cd build/application && rm -rf Res && rm -f log_* && exec mpirun -np 2 FlowShopEO @FlowShopEO.param)