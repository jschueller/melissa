#!/bin/bash

#SBATCH -N ${NODES_SIMU}
#SBATCH --ntasks-per-node=${PROC_PER_NODE}
#SBATCH --partition=fast
#SBATCH --time=${WALLTIME_SIMU}
#SBATCH -o simu.%j.log
#SBATCH -e simu.%j.err
#SBATCH --job-name=Simu${SIMU_ID}


module load openmpi


source /home/users/pogodzinski/melissa/install/bin/melissa_set_env.sh
export MELISSA_SERVER_NODE_NAME=${SERVER_NODE_NAME}
export MELISSA_MASTER_NODE_NAME=${HOSTNAME}
date +\"%d/%m/%y %T\"
${SIMU_CMD}
date +\"%d/%m/%y %T\"
