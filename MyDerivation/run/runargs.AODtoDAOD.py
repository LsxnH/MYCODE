# Run arguments file auto-generated on Fri May  3 23:36:27 2019 by:
# JobTransform: AODtoDAOD
# Version: $Id: trfExe.py 792052 2017-01-13 13:36:51Z mavogel $
# Import runArgs class
from PyJobTransforms.trfJobOptions import RunArguments
runArgs = RunArguments()
runArgs.trfSubstepName = 'AODtoDAOD' 

runArgs.maxEvents = 100
runArgs.reductionConf = ['MUON5']

# Input data
runArgs.inputAODFile = ['/lustre/AtlUser/liheng/data/WH_AOD_mc16a/mc16_13TeV.361107.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zmumu.merge.AOD.e3601_s3126_r9364_r9315/AOD.11182597._000557.pool.root.1']
runArgs.inputAODFileType = 'AOD'
runArgs.inputAODFileNentries = 10000L
runArgs.AODFileIO = 'input'

# Output data
runArgs.outputDAOD_MUON5File = 'DAOD_MUON5.output.pool.root'
runArgs.outputDAOD_MUON5FileType = 'AOD'

# Extra runargs

# Extra runtime runargs

# Literal runargs snippets
