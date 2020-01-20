#!/bin/bash

# (See https://www.psc.edu/bridges/user-guide/running-jobs for command details)


# Specify the number of nodes:
#SBATCH -N 1

# Specify using a "Regular Memory" node, allowing to share a node with other jobs:
# (If you don't specify "-shared", you get allocated all 28 cores per node, and our allocation is charged for all of them.)
#SBATCH -p RM-shared

# Specify the number of cores:
#SBATCH --ntasks-per-node 7

# Specify your max walltime:
#SBATCH -t 0:10:00


# Replace <username> below with your PSC username.

# Move to your working directory (in pylon5):
cd /pylon5/sc31l6p/xingyuw
# Load your program:
cp $HOME/587-final-project/NQueens_OpenMP_random .

# Run your OpenMP program
# (">" redirects the print output of your program,
#  in this case to "NQueens_OpenMP.txt" in pylon5)

export OMP_NUM_THREADS=7
./NQueens_OpenMP_random 8 > /pylon5/sc31l6p/xingyuw/NQueens_OpenMP_8_t7_random.txt


