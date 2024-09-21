export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
source ${AtlasSetup}/scripts/asetup.sh AtlasProduction,21.0.20.1,here

source $TestArea/../build/$LCG_PLATFORM/setup.sh	

export MAKEFLAGS="-j4"
echo "MAKEFLAGS=$MAKEFLAGS"