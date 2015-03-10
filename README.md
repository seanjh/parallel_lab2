# Parallel Lab 2
## Setup Instructions on AWS
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

###Update the LD_LIBRARY_PATY environment variable
Update LD_LIBRARY_PATY in .bash_profile, or execute the following command.

    export LD_LIBRARY_PATH=$HOME/gmplib/lib:$LD_LIBRARY_PATH
    
## Execution instructions


### Activate devtoolset-2 
    scl enable devtoolset-2 bash

### Compile the project

    make divisors_app

    mpic++ -std=c++11 -o divisors_app obj/gmp_factors_API.o obj/DivisorApplication.o obj/DivisorResult.o obj/DivisorWork.o obj/MW_API.o obj/MW_Master.o obj/MW_Worker.o -I/home/cluster/gmplib/include -L/home/cluster/gmplib/lib -lgmpxx -lgmp


### Execute the program

    bash ./mw_test.sh
