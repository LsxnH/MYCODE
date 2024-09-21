# echo "setup PhysicsAnpRPC PhysicsAnpRPC-v1 in /home/hengli/testarea/RPCPerf/PhysicsNtuple"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/CMT/v1r25p20160527
endif
source ${CMTROOT}/mgr/setup.csh
set cmtPhysicsAnpRPCtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtPhysicsAnpRPCtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=PhysicsAnpRPC -version=PhysicsAnpRPC-v1 -path=/home/hengli/testarea/RPCPerf/PhysicsNtuple  -no_cleanup $* >${cmtPhysicsAnpRPCtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe setup -csh -pack=PhysicsAnpRPC -version=PhysicsAnpRPC-v1 -path=/home/hengli/testarea/RPCPerf/PhysicsNtuple  -no_cleanup $* >${cmtPhysicsAnpRPCtempfile}"
  set cmtsetupstatus=2
  /bin/rm -f ${cmtPhysicsAnpRPCtempfile}
  unset cmtPhysicsAnpRPCtempfile
  exit $cmtsetupstatus
endif
set cmtsetupstatus=0
source ${cmtPhysicsAnpRPCtempfile}
if ( $status != 0 ) then
  set cmtsetupstatus=2
endif
/bin/rm -f ${cmtPhysicsAnpRPCtempfile}
unset cmtPhysicsAnpRPCtempfile
exit $cmtsetupstatus

