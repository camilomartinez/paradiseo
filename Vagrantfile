# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # Assign the box to start from:
  config.vm.box = "ubuntu/trusty64"

  # Assign where to find the box if not already available locally:
  config.vm.box_url = "https://atlas.hashicorp.com/ubuntu/trusty64"

  # Tell ubuntu to use a closer mirror for packages
  config.vm.provision :shell, :inline => "sed -i 's/us.archive/de.archive/g' /etc/apt/sources.list"
  # Tell Vagrant what commands to run to provision our box
  config.vm.provision :shell, inline: "apt-get -y update"
  # Paradiseo dependencies
  config.vm.provision :shell, inline: "sudo apt-get -y install g++ cmake make doxygen lcov"
  #Openmpi
  config.vm.provision :shell, inline: "sudo apt-get -y install openmpi-bin openmpi-doc libopenmpi-dev"

end