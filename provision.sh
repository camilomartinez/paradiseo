#!/usr/bin/env bash

# Tell ubuntu to use a closer mirror for packages
sed -i 's/us.archive/de.archive/g' /etc/apt/sources.list
# Tell Vagrant what commands to run to provision our box
apt-get -y update
# Paradiseo dependencies
apt-get -y install g++ cmake make doxygen lcov
#Openmpi
apt-get -y install openmpi-bin openmpi-doc libopenmpi-dev

# Copy policloud access files
cp /vagrant/config /home/vagrant/.ssh/
cp /vagrant/ubuntu-cluster-private.pem /home/vagrant/.ssh/ubuntu-cluster-private.pem
# Proper permisisons
chmod 400 /home/vagrant/.ssh/config
chmod 400 /home/vagrant/.ssh/ubuntu-cluster-private.pem
chown vagrant /home/vagrant/.ssh/config
chown vagrant /home/vagrant/.ssh/ubuntu-cluster-private.pem
