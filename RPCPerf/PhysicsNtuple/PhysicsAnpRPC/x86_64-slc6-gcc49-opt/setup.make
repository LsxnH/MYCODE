----------> uses
# use AtlasPolicy AtlasPolicy-*  (no_version_directory)
#   use ExternalPolicy ExternalPolicy-* External (no_version_directory)
#     use PlatformPolicy PlatformPolicy-* External (no_version_directory)
#       use GaudiPolicy *  (no_version_directory)
#         use LCG_Settings *  (no_version_directory)
#         use Python * LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=2.7.9.p1)
#         use tcmalloc * LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=2.4)
#           use LCG_Configuration v*  (no_version_directory)
#           use LCG_Settings v*  (no_version_directory)
#           use libunwind v* LCG_Interfaces (no_version_directory) (native_version=5c2cade)
#             use LCG_Configuration v*  (no_version_directory)
#             use LCG_Settings v*  (no_version_directory)
#         use Reflex v* LCG_Interfaces (no_auto_imports) (no_version_directory)
#           use LCG_Configuration v*  (no_version_directory)
#           use LCG_Settings v*  (no_version_directory)
#           use ROOT v* LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=6.04.16)
#       use AtlasCommonPolicy AtlasCommonPolicy-*  (no_version_directory)
#         use LCG_Platforms *  (no_version_directory)
#         use AtlasDoxygen AtlasDoxygen-* Tools (no_version_directory) (native_version=)
#           use LCG_Settings *  (no_version_directory)
#           use LCG_Configuration *  (no_version_directory)
#             use LCG_Platforms *  (no_version_directory)
#     use Mac105_Compat Mac105_Compat-* External (no_version_directory) (native_version=1.0.0)
#       use AtlasExternalArea AtlasExternalArea-* External (no_version_directory)
#         use LCG_Settings v*  (no_version_directory)
#         use PlatformPolicy PlatformPolicy-* External (no_version_directory)
#       use PlatformPolicy PlatformPolicy-* External (no_version_directory)
#   use AtlasCxxPolicy AtlasCxxPolicy-*  (no_version_directory)
#     use GaudiPolicy v*  (no_auto_imports) (no_version_directory)
#     use ExternalPolicy ExternalPolicy-* External (no_version_directory)
#     use AtlasCompilers AtlasCompilers-* External (no_version_directory)
#       use LCG_Platforms *  (no_version_directory)
#     use CheckerGccPlugins CheckerGccPlugins-* External (no_version_directory) (native_version=CheckerGccPlugins-00-01-84)
#       use ExternalPolicy ExternalPolicy-* External (no_version_directory)
#       use AtlasCompilers AtlasCompilers-* External (no_version_directory)
#   use AtlasCommonPolicy AtlasCommonPolicy-*  (no_version_directory)
#   use GaudiPolicy v*  (no_auto_imports) (no_version_directory)
#   use CodeCheck CodeCheck-* Tools (no_version_directory)
#   use AtlasDoxygen AtlasDoxygen-* Tools (no_version_directory) (native_version=)
# use AtlasROOT AtlasROOT-* External (no_version_directory)
#   use ExternalPolicy ExternalPolicy-* External (no_version_directory)
#   use ROOT v* LCG_Interfaces (no_version_directory) (native_version=6.04.16)
#     use LCG_Configuration v*  (no_version_directory)
#     use LCG_Settings v*  (no_version_directory)
#     use GCCXML v* LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=0.9.0_20131026)
#       use LCG_Configuration v*  (no_version_directory)
#       use LCG_Settings v*  (no_version_directory)
#     use Python v* LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=2.7.9.p1)
#     use xrootd v* LCG_Interfaces (no_version_directory) (native_version=4.2.3)
#       use LCG_Configuration v*  (no_version_directory)
#       use LCG_Settings v*  (no_version_directory)
# use AtlasBoost AtlasBoost-* External (no_version_directory)
#   use AtlasPolicy AtlasPolicy-*  (no_version_directory)
#   use Boost v* LCG_Interfaces (no_version_directory) (native_version=1.59.0_python2.7)
#     use LCG_Configuration v*  (no_version_directory)
#     use LCG_Settings v*  (no_version_directory)
#     use Python v* LCG_Interfaces (no_auto_imports) (no_version_directory) (native_version=2.7.9.p1)
# use AtlasPython AtlasPython-* External (no_version_directory)
#   use ExternalPolicy ExternalPolicy-00-* External (no_version_directory)
#   use Python v* LCG_Interfaces (no_version_directory) (native_version=2.7.9.p1)
#     use LCG_Configuration v*  (no_version_directory)
#     use LCG_Settings v*  (no_version_directory)
# use AtlasReflex AtlasReflex-* External (no_version_directory)
#   use ExternalPolicy ExternalPolicy-* External (no_version_directory)
#   use AtlasRELAX AtlasRELAX-* External (no_auto_imports) (no_version_directory)
#     use ExternalPolicy ExternalPolicy-* External (no_version_directory)
#     use RELAX v* LCG_Interfaces (no_version_directory) (native_version=RELAX-root6)
#       use LCG_Configuration v*  (no_version_directory)
#       use LCG_Settings v*  (no_version_directory)
#   use AtlasROOT AtlasROOT-* External (no_version_directory)
#
# Selection :
use CMT v1r25p20160527 (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30)
use CodeCheck CodeCheck-01-02-03 Tools (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use LCG_Platforms v1  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use AtlasCompilers AtlasCompilers-00-00-25 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use LCG_Configuration v1  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use LCG_Settings v1  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use RELAX v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e) (no_auto_imports)
use AtlasDoxygen AtlasDoxygen-00-04-10 Tools (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasCommonPolicy AtlasCommonPolicy-00-00-56-01  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use xrootd v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use GCCXML v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e) (no_auto_imports)
use libunwind v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e) (no_auto_imports)
use tcmalloc v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e) (no_auto_imports)
use Python v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use Boost v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use ROOT v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e)
use Reflex v1 LCG_Interfaces (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e) (no_auto_imports)
use GaudiPolicy GaudiPolicy-15-05-00  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/GAUDI/v26r2p1-lcg81e)
use PlatformPolicy PlatformPolicy-00-00-20 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasExternalArea AtlasExternalArea-00-00-27 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use Mac105_Compat Mac105_Compat-00-00-01 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use ExternalPolicy ExternalPolicy-00-01-72 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasRELAX AtlasRELAX-00-01-00 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30) (no_auto_imports)
use AtlasPython AtlasPython-00-01-07 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasROOT AtlasROOT-02-03-28 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasReflex AtlasReflex-00-03-10 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use CheckerGccPlugins CheckerGccPlugins-00-01-85 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasCxxPolicy AtlasCxxPolicy-00-00-77  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasPolicy AtlasPolicy-01-08-46-04  (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use AtlasBoost AtlasBoost-00-00-11 External (/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30)
use CMTUSERCONTEXT v0 (/cvmfs/atlas.cern.ch/repo/tools/slc6/cmt)
----------> tags
x86_64-slc6-gcc49-opt (from CMTCONFIG) package [LCG_Platforms PlatformPolicy] implies [target-x86_64 target-slc6 target-gcc49 target-opt Linux slc6 64 gcc-4.9 opt x86_64-slc6-gcc49]
ATLAS (from arguments) package [GaudiPolicy] implies [use-shared-dir no-pyzip] applied [ExternalPolicy]
ManaCore (from arguments)
AthAnalysisBase (from arguments)
CMTv1 (from CMTVERSION)
CMTr25 (from CMTVERSION)
CMTp20160527 (from CMTVERSION)
Linux (from uname) package [CMT LCG_Platforms AtlasCxxPolicy AtlasPolicy] implies [Unix host-linux cpp_native_dependencies]
STANDALONE (from CMTSITE)
CMTUSERCONTEXT_no_config (from PROJECT) excludes [CMTUSERCONTEXT_config]
CMTUSERCONTEXT_no_root (from PROJECT) excludes [CMTUSERCONTEXT_root]
CMTUSERCONTEXT_cleanup (from PROJECT) excludes [CMTUSERCONTEXT_no_cleanup]
CMTUSERCONTEXT_scripts (from PROJECT) excludes [CMTUSERCONTEXT_no_scripts]
CMTUSERCONTEXT_no_prototypes (from PROJECT) excludes [CMTUSERCONTEXT_prototypes]
CMTUSERCONTEXT_with_installarea (from PROJECT) excludes [CMTUSERCONTEXT_without_installarea]
CMTUSERCONTEXT_without_version_directory (from PROJECT) excludes [CMTUSERCONTEXT_with_version_directory]
testarea_no_config (from PROJECT) excludes [testarea_config]
testarea_no_root (from PROJECT) excludes [testarea_root]
testarea_cleanup (from PROJECT) excludes [testarea_no_cleanup]
testarea_scripts (from PROJECT) excludes [testarea_no_scripts]
testarea_no_prototypes (from PROJECT) excludes [testarea_prototypes]
testarea_with_installarea (from PROJECT) excludes [testarea_without_installarea]
testarea_without_version_directory (from PROJECT) excludes [testarea_with_version_directory]
testarea (from PROJECT)
AthAnalysisBase_no_config (from PROJECT) excludes [AthAnalysisBase_config]
AthAnalysisBase_no_root (from PROJECT) excludes [AthAnalysisBase_root]
AthAnalysisBase_cleanup (from PROJECT) excludes [AthAnalysisBase_no_cleanup]
AthAnalysisBase_scripts (from PROJECT) excludes [AthAnalysisBase_no_scripts]
AthAnalysisBase_no_prototypes (from PROJECT) excludes [AthAnalysisBase_prototypes]
AthAnalysisBase_with_installarea (from PROJECT) excludes [AthAnalysisBase_without_installarea]
AthAnalysisBase_without_version_directory (from PROJECT) excludes [AthAnalysisBase_with_version_directory]
GAUDI_no_config (from PROJECT) excludes [GAUDI_config]
GAUDI_root (from PROJECT) excludes [GAUDI_no_root]
GAUDI_cleanup (from PROJECT) excludes [GAUDI_no_cleanup]
GAUDI_scripts (from PROJECT) excludes [GAUDI_no_scripts]
GAUDI_prototypes (from PROJECT) excludes [GAUDI_no_prototypes]
GAUDI_with_installarea (from PROJECT) excludes [GAUDI_without_installarea]
GAUDI_without_version_directory (from PROJECT) excludes [GAUDI_with_version_directory]
LCGCMT_no_config (from PROJECT) excludes [LCGCMT_config]
LCGCMT_no_root (from PROJECT) excludes [LCGCMT_root]
LCGCMT_cleanup (from PROJECT) excludes [LCGCMT_no_cleanup]
LCGCMT_scripts (from PROJECT) excludes [LCGCMT_no_scripts]
LCGCMT_prototypes (from PROJECT) excludes [LCGCMT_no_prototypes]
LCGCMT_with_installarea (from PROJECT) excludes [LCGCMT_without_installarea]
LCGCMT_without_version_directory (from PROJECT) excludes [LCGCMT_with_version_directory]
x86_64 (from package CMT) package [LCG_Platforms] implies [host-x86_64] applied [CMT]
sl65 (from package CMT) package [PlatformPolicy] implies [host-slc6] applied [CMT]
gcc493 (from package CMT) applied [CMT]
Unix (from package CMT) package [LCG_Platforms] implies [host-unix] excludes [WIN32 Win32]
c_native_dependencies (from package CMT) activated GaudiPolicy
cpp_native_dependencies (from package CMT) activated GaudiPolicy
/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30:/cvmfs/atlas.cern.ch/repo/sw/tdaq/tdaqnotProject (from package ExternalPolicy) applied [ExternalPolicy]
target-unix (from package LCG_Settings) activated LCG_Platforms
target-x86_64 (from package LCG_Settings) activated LCG_Platforms
target-gcc49 (from package LCG_Settings) package [LCG_Platforms] implies [target-gcc4 target-lcg-compiler lcg-compiler target-c11] activated LCG_Platforms
host-x86_64 (from package LCG_Settings) activated LCG_Platforms
target-slc (from package LCG_Settings) package [LCG_Platforms] implies [target-linux] activated LCG_Platforms
target-gcc (from package LCG_Settings) activated LCG_Platforms
target-gcc4 (from package LCG_Settings) package [LCG_Platforms] implies [target-gcc] activated LCG_Platforms
target-lcg-compiler (from package LCG_Settings) activated LCG_Platforms
host-linux (from package LCG_Platforms) package [LCG_Platforms] implies [host-unix]
host-unix (from package LCG_Platforms)
host-slc6 (from package LCG_Platforms) package [LCG_Platforms] implies [host-slc] activated PlatformPolicy
host-slc (from package LCG_Platforms) package [LCG_Platforms] implies [host-linux] activated PlatformPolicy
target-opt (from package LCG_Platforms)
target-slc6 (from package LCG_Platforms) package [LCG_Platforms] implies [target-slc]
target-linux (from package LCG_Platforms) package [LCG_Platforms] implies [target-unix]
lcg-compiler (from package LCG_Platforms)
target-c11 (from package LCG_Platforms)
ROOT_GE_5_15 (from package LCG_Configuration) applied [LCG_Configuration]
ROOT_GE_5_19 (from package LCG_Configuration) applied [LCG_Configuration]
ROOT_GE_6_00 (from package LCG_Configuration) applied [LCG_Configuration]
use-shared-dir (from package GaudiPolicy)
do_genconf (from package GaudiPolicy) applied [AtlasPolicy] activated AtlasPolicy
no-pyzip (from package GaudiPolicy)
separate-debug (from package AtlasCommonPolicy) excludes [no-separate-debug] applied [AtlasCommonPolicy]
NICOS (from package AtlasCommonPolicy) package AtlasCommonPolicy excludes [NICOSrel_nightly] applied [AtlasCommonPolicy]
LCGCMT_INSTALLED (from package PlatformPolicy) applied [PlatformPolicy ExternalPolicy]
block-tdaqc (from package PlatformPolicy) applied [PlatformPolicy ExternalPolicy]
opt (from package PlatformPolicy) package [AtlasPolicy] implies [optimized]
gcc (from package PlatformPolicy)
64 (from package PlatformPolicy) package [PlatformPolicy] implies [target-64]
gcc-4.9 (from package PlatformPolicy) package [PlatformPolicy] implies [gcc49x gcc49 gcc]
gcc49x (from package PlatformPolicy)
gcc49 (from package PlatformPolicy)
slc6 (from package PlatformPolicy)
x86_64-slc6-gcc49 (from package PlatformPolicy)
target-64 (from package PlatformPolicy)
asNeeded (from package PlatformPolicy) applied [PlatformPolicy]
CheckerGccPlugins_project_AthAnalysisBase (from package CheckerGccPlugins) applied [CheckerGccPlugins]
optimized (from package AtlasPolicy) package [AtlasPolicy] implies [opt]
HasAthenaRunTime (from package AtlasPolicy) applied [AtlasPolicy]
SITE_ (from package AtlasPolicy) applied [AtlasPolicy]
ROOTBasicLibs (from package AtlasROOT) applied [AtlasROOT PhysicsAnpRPC]
ROOTMathLibs (from package AtlasROOT) applied [PhysicsAnpRPC] activated PhysicsAnpRPC
ROOTGraphicsLibs (from package AtlasROOT) applied [PhysicsAnpRPC] activated PhysicsAnpRPC
no_pack_dist_check (from package PhysicsAnpRPC) applied [PhysicsAnpRPC]
----------> CMTPATH
# Add path /cvmfs/atlas.cern.ch/repo/tools/slc6/cmt from CMTUSERCONTEXT
# Add path /hepustc/home/hengli/testarea/RPCPerf from initialization
# Add path /cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/AthAnalysisBase/2.4.30 from initialization
# Add path /cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/GAUDI/v26r2p1-lcg81e from ProjectPath
# Add path /cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.30/LCGCMT/LCGCMT_81e from ProjectPath
