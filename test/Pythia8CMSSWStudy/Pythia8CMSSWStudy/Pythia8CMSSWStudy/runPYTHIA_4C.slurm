#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=2G
#SBATCH --output=pythia8_QCDana.slurm.o%A
#SBATCH --time=0-18:00:00
#SBATCH --job-name="pythia8_QCDana"

source /cvmfs/cms.cern.ch/cmsset_default.sh
export SCRAM_ARCH=slc6_amd64_gcc491
eval `scramv1 runtime -sh`

cmsRun anaQCD_PYTHIA8_cfg.py output="$CMSRUN_OUTPUT" maxEvents="$CMSRUN_MAXEVENTS" tune="$CMSRUN_TUNE" processType="$CMSRUN_PROCESSTYPE" ptHatLow="$CMSRUN_PTHATLOW" ptHatHigh="$CMSRUN_PTHATHIGH" sqrtS="$CMSRUN_ENERGY" &> run-$CMSRUN_ENERGY-$CMSRUN_PTHATLOW-$CMSRUN_PTHATHIGH-$CMSRUN_TUNE.log

exit 0
