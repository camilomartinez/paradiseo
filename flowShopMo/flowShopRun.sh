#!/bin/bash

(cd build/application && exec mpirun -np 3  FlowShopMOEA @FlowShopMOEA.param)