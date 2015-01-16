#!/bin/bash

#SBATCH --time=00:10:00   # walltime
#SBATCH --ntasks=4   # number of processor cores (i.e. tasks)
#SBATCH --nodes=1   # number of nodes
#SBATCH --mem-per-cpu=1024M   # memory per CPU core
#SBATCH -J "hot_plate_openmp"   # job name
#SBATCH --mail-user=anderson.peter@byu.edu   # email address
#SBATCH --mail-type=BEGIN

# Compatibility variables for PBS. Delete if not needed.
export PBS_NODEFILE=`/fslapps/fslutils/generate_pbs_nodefile`
export PBS_JOBID=$SLURM_JOB_ID
export PBS_O_WORKDIR="$SLURM_SUBMIT_DIR"
export PBS_QUEUE=batch

# Set the max number of threads to use for programs using OpenMP. Should be <= ppn. Does nothing if the program doesn't use OpenMP.
export OMP_NUM_THREADS=$SLURM_CPUS_ON_NODE

cp hot_plate_openmp.c /tmp/$SLURM_JOB_ID/.
cd /tmp/$SLURM_JOB_ID
gcc hotplate_openmp.c -Wall -fopenmp -o plate
./plate


# LOAD MODULES, INSERT CODE, AND RUN YOUR PROGRAMS HERE
