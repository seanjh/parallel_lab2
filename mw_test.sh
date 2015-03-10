#!/usr/bin/env bash
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 4 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 5 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 6 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 7 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 8 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 9 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 10 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 11 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 12 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 13 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 14 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 15 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 16 -hostfile ../hosts divisors_app 500000000000000

mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000 1
mpirun -n 4 -hostfile ../hosts divisors_app 500000000000000 10
mpirun -n 8 -hostfile ../hosts divisors_app 500000000000000 100
mpirun -n 12 -hostfile ../hosts divisors_app 500000000000000 1000
mpirun -n 16 -hostfile ../hosts divisors_app 500000000000000 10000
