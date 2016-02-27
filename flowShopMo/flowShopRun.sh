#!/bin/bash


(cd build/application && rm -rf Res && rm -f log_w_* && exec mpirun -np 6  FlowShopMOEA @FlowShopMOEA.param)