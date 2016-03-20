#!/bin/bash

# Running on 6 nodes
# 1 master and 5 compute, 8 core each.

(cd application && \
 rm -rf Res && \
 rm -f log_* && \
 time mpirun --hostfile hosts -np 21 --bynode FlowShopEO @FlowShopEO.param)
