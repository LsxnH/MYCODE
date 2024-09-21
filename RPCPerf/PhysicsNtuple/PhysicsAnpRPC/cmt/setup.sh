# echo "setup PhysicsAnpRPC PhysicsAnpRPC-v1 in /home/hengli/testarea/RPCPerf/PhysicsNtuple"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/CMT/v1r25p20160527; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtPhysicsAnpRPCtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtPhysicsAnpRPCtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=PhysicsAnpRPC -version=PhysicsAnpRPC-v1 -path=/home/hengli/testarea/RPCPerf/PhysicsNtuple  -no_cleanup $* >${cmtPhysicsAnpRPCtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe setup -sh -pack=PhysicsAnpRPC -version=PhysicsAnpRPC-v1 -path=/home/hengli/testarea/RPCPerf/PhysicsNtuple  -no_cleanup $* >${cmtPhysicsAnpRPCtempfile}"
  cmtsetupstatus=2
  /bin/rm -f ${cmtPhysicsAnpRPCtempfile}
  unset cmtPhysicsAnpRPCtempfile
  return $cmtsetupstatus
fi
cmtsetupstatus=0
. ${cmtPhysicsAnpRPCtempfile}
if test $? != 0 ; then
  cmtsetupstatus=2
fi
/bin/rm -f ${cmtPhysicsAnpRPCtempfile}
unset cmtPhysicsAnpRPCtempfile
return $cmtsetupstatus

