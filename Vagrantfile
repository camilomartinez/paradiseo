# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # Assign the box to start from:
  config.vm.box = "ubuntu/trusty64"

  # Assign where to find the box if not already available locally:
  config.vm.box_url = "https://atlas.hashicorp.com/ubuntu/trusty64"

  # How to provision the box
  config.vm.provision :shell, path: "provision.sh"

  # Configure master
  node_name = "master"
  config.vm.provider :virtualbox do |vb|
    vb.name = node_name
    # Resources
    vb.memory = 512
    vb.cpus = 2
  end
  config.vm.hostname = node_name

end