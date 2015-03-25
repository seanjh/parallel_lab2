#!/usr/bin/env bash
# mpirun -n 16 ./divisors_app 500000000000000000 10000
mpirun -n 32 ./divisors_app 500000000000000000 10000 .05
mpirun -n 32 ./divisors_app 500000000000000000 10000 .01
mpirun -n 32 ./divisors_app 500000000000000000 10000 .005
mpirun -n 32 ./divisors_app 500000000000000000 10000 .001