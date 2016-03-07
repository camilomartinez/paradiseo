#!/usr/bin/env bash

# Tell ubuntu to use a closer mirror for packages
sed -i 's/us.archive/de.archive/g' /etc/apt/sources.list
# Tell Vagrant what commands to run to provision our box
apt-get -y update
# Paradiseo dependencies
apt-get -y install g++ cmake make doxygen lcov
#Openmpi
apt-get -y install openmpi-bin openmpi-doc libopenmpi-dev
