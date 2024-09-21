# echo "cleanup PhysicsAnpRPC PhysicsAnpRPC-v1 in /home/hengli/testarea/RPCPerf/PhysicsNtuple"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtPhysicsAnpRPCtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtPhysicsAnpRPCtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=PhysicsAnpRPC -version=PhysicsAnpRPC-v1 -path=/home/hengli/testarea/RPCPerf/PhysicsNtuple  $* >${cmtPhysicsAnpRPCtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=PhysicsAnpRPC -version=PhysicsAnpRPC-v1 -path=/home/hengli/testarea/RPCPerf/PhysicsNtuple  $* >${cmtPhysicsAnpRPCtempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtPhysicsAnpRPCtempfile}
  unset cmtPhysicsAnpRPCtempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtPhysicsAnpRPCtempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtPhysicsAnpRPCtempfile}
unset cmtPhysicsAnpRPCtempfile
return $cmtcleanupstatus

