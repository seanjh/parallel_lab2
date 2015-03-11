#!/usr/bin/env bash

echo START_TEST:2,500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 2 -hostfile ../hosts divisors_app 500000000000000

echo START_TEST:3,500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000
mpirun -n 3 -hostfile ../hosts divisors_app 500000000000000

