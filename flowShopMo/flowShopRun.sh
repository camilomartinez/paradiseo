#!/bin/bash


(cd build/application && rm -rf Res && exec mpirun -np 2  FlowShopMOEA @FlowShopMOEA.param)