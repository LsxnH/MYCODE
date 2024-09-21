# Declare the package name:
atlas_subdir( PhysicsAnpRPC )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
			  Control/AthenaBaseComps
			  Control/AthContainers
			  External/GaudiInterface	  
			  External/AtlasROOT	  
			  Trigger/TrigEvent/TrigSteeringEvent
			  Trigger/TrigAnalysis/TrigMuonMatching
			  Event/xAOD/xAODMuon
			  Event/xAOD/xAODCore
			  Event/xAOD/xAODTrigger
			  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          PRIVATE
			  MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
			  MuonSpectrometer/MuonIdHelpers
			  MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonTGRecTools
			  MuonSpectrometer/MuonCablings/MuonRPC_Cabling
			  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack 
			  MuonSpectrometer/MuonRDO
			  Tracking/TrkEvent/TrkMeasurementBase
			  Tracking/TrkEvent/TrkRIO_OnTrack
			  Tracking/TrkExtrapolation/TrkExInterfaces
			  DetectorDescription/GeoPrimitives
			  Event/xAOD/xAODEventInfo
			  Event/xAOD/xAODTrigger
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigT1/TrigT1Result 
			  PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections 
			  PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
			  PhysicsAnalysis/MuonID/MuonSelectorTools
			  PhysicsAnalysis/AnalysisCommon/IsolationSelection 
			  )

# External dependencies:
find_package( Eigen )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( PhysicsAnpRPC
                     src/*.cxx
                     src/components/*.cxx
		     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} ${CLHEP_LIBRARIES} ${CORAL_LIBRARIES} GaudiKernel 
		     AthenaBaseComps AthContainers xAODCore xAODTrigger Identifier TrkTrack CaloEvent 
		     TrkDistortedSurfaces MuonIdHelpers xAODEgamma xAODTrigEgamma xAODTau xAODTrigMuon
		     TrigSteeringEvent TrkVolumes MuonRIO_OnTrack TrigDecisionTool MuonRPC_Cabling 
		     TrigConfL1Data TrigT1Interfaces TrigT1Result MuonRDO )

# Install files from the package:
atlas_install_headers( PhysicsAnpRPC )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/* )
atlas_install_python_modules( python/*.py )
