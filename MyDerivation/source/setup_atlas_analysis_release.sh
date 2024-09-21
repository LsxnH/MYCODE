myrelease=21.2.51.0

export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh

if [ -e "../build" ]
then
    pushd ../build
    acmSetup --sourcedir=../source AthDerivation,$myrelease
    popd
else
    echo "Error: build directory does not exist"
fi

echo "---------------------------------------------------------------"
echo "Using AthDerivation ${myrelease}"
echo "MAKEFLAGS=$MAKEFLAGS"

