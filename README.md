# Parallel Lab 2
## Configuring a new AWS cluster instance
The CentOS 5 system configured for this course does not include the necessary C++ compiler. It is also missing several key libraries. The instructions below will configure a new AWS Centos 5 image to allow for this project to be compiled. To run the application on an machine already set up, skip to Execution instructions.

### Enable devtoolset-2

    cd /etc/yum.repos.d/
    wget -O /etc/yum.repos.d/slc5-devtoolset.repo http://linuxsoft.cern.ch/cern/devtoolset/slc5-devtoolset.repo
    yum install --nogpgcheck devtoolset-2
    scl enable devtoolset-2 bash

If there are errors relating to GPG keys, then edit the /etc/yum.repos.d/slc5-devtoolset.repo file. Switch all the gpgcheck values to 0.

### Install an updated version GMP
The version of GMP available through yum is not recent enough. Download and extract a more recent version to the home directory. Compile as follows:

    ./configure --enable-cxx --prefix=$HOME/gmplib

    make install
    
## Execution instructions on cofigured cluster

### Activate devtoolset-2 
    
    scl enable devtoolset-2 bash

### Compile as root

cd /home/cluster/parallel_lab2
make divisors_app

### Switch to the cluster user

    su cluster
    cd ~/parallel_lab2

Note, a valid hostname file is required in the directory below (..) the project directory.

### Make certain that LD_LIBRARY_PATH is configured
The LD_LIBRARY_PATH environment variable must include a reference to the custom gmp install.

    export LD_LIBRARY_PATH=$HOME/gmplib/lib:$LD_LIBRARY_PATH

### Compile the executable
This command links the objects to the custom GMP installation.

    mpic++ -std=c++11 -o divisors_app obj/gmp_factors_API.o obj/DivisorApplication.o obj/DivisorResult.o obj/DivisorWork.o obj/MW_API.o obj/MW_Master.o obj/MW_Worker.o -I/home/cluster/gmplib/include -L/home/cluster/gmplib/lib -lgmpxx -lgmp

### Execute the program

    bash ./mw_test.sh
