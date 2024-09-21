// -*- c++ -*-
#ifndef ANP_L1MUONANALYSIS_H
#define ANP_L1MUONANALYSIS_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : L1MuonAnalysis
 * @Author : Stefano Rosati
 *
 * @Brief  : This class is copied from MuonTriggerValidation
 * 
 **********************************************************************************/

// Athena/Gaudi
#include "AthenaBaseComps/AthHistogramAlgorithm.h"
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "TrigMuonMatching/ITrigMuonMatching.h"
#include "GaudiKernel/ITHistSvc.h"

// Local
#include "L1MuonExtrapolator.h"
#include "L1MuonHistoTool.h"

// ROOT
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"

// C/C++
#include <map>
#include <vector>
#include <utility>
#include <string>

class RpcIdHelper;
class CscIdHelper;
class Identifier;

namespace Muon 
{
  class MuonIdHelperTool;
}

namespace MuonGM 
{
  class MuonDetectorManager;
}

class L1MuonAnalysis : public AthHistogramAlgorithm
{
 public:

  L1MuonAnalysis( const std::string& name, ISvcLocator* pSvcLocator );
  ~L1MuonAnalysis();

  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

 private:

  // book histos
  StatusCode bookTree();
  void bookHistos();

  // check the roi words in the muon roi
  void checkMuonRoi();

  // fill the ntuple information with 
  void fillPRDContainer();

  // selection functions
  void selectSingleMuons(bool& selected);
  void selectZmumu(bool& selected);

  StatusCode selectGoodMuons();
  void selectMuonPairs();
  
  bool triggerMatch(const xAOD::Muon* mu);
  void analyseMuon(const xAOD::Muon* mu);
  void checkTriggerMatch(const xAOD::Muon* mu);

  bool checkOrthoTrigger();
  bool checkZeroBiasTrigger();

 private:

  // run properties
  BooleanProperty m_doTagAndProbe;
  BooleanProperty m_doOrthogonal;
  BooleanProperty m_doZeroBias;
  BooleanProperty m_doFullPRD;
  BooleanProperty m_doNtuple;
  BooleanProperty m_doHistos;
  BooleanProperty m_doCosmics;

  StoreGateSvc* m_storeGate;
  
  const MuonGM::MuonDetectorManager* m_muonDetMgr;

  std::string m_triggerChainGroupRegExp;

  // output tree
  //  TFile* m_outFile;
  ITHistSvc * m_thistSvc; 
  TTree* m_tree;
  const RpcIdHelper* m_rpcIdHelper;
  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;

  // trigger tools
  ToolHandle<Trig::ITrigMuonMatching> m_matchTool;
  ToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;

  const Trig::ChainGroup* m_chainGroup;
  const Trig::ChainGroup* m_chainGroupZeroBias;
  const Trig::ChainGroup* m_chainGroupZeroBiasJ40;
  const Trig::ChainGroup* m_chainGroupEmpty;

  ToolHandle<L1MuonExtrapolator>  m_muonExtrapolator;  // the muon extrapolator  
  ToolHandle<L1MuonHistoTool>     m_muonHistoTool;     // the tool for efficiency histograms
 
  // preselected containers
  std::vector<const xAOD::Muon*>              m_preselMuons;
  std::vector<std::vector<const xAOD::Muon*>> m_muonPairs;
  std::vector<const xAOD::Muon*>              m_bestPair;
 
  //
  // Tree variables
  //
  int m_nProbes;
  int m_nTags;
  int m_nExpHits;

  float m_diMuonMass;
  float m_deltaPhi;
  std::vector<float> m_tagEta;
  std::vector<float> m_tagPhi;
  std::vector<float> m_tagPt;

  std::vector<float> m_probeEta;
  std::vector<float> m_probePhi;
  std::vector<float> m_probePt;
  std::vector<float> m_probeQOverPErr;
  
  std::vector<int> m_matchTh1;
  std::vector<int> m_matchTh2;
  std::vector<int> m_matchTh3;
  std::vector<int> m_matchTh4;
  std::vector<int> m_matchTh5;
  std::vector<int> m_matchTh6;

  // positions of the expected intersections
  std::vector<float> m_exphitpos_x;
  std::vector<float> m_exphitpos_y;
  std::vector<float> m_exphitpos_z;

  // identifiers of the expected intersections
  std::vector<int> m_stationName;
  std::vector<int> m_stationEta;
  std::vector<int> m_stationPhi;
  std::vector<int> m_doubletR;
  std::vector<int> m_doubletZ;
  std::vector<int> m_doubletPhi;
  std::vector<int> m_gasGap;
  std::vector<int> m_measPhi;
  std::vector<int> m_strip;  
  std::vector<float> m_residuals;

  // identifiers of the closest hit
  std::vector<int> m_hitStationName;
  std::vector<int> m_hitStationEta;
  std::vector<int> m_hitStationPhi;
  std::vector<int> m_hitDoubletR;
  std::vector<int> m_hitDoubletZ;
  std::vector<int> m_hitDoubletPhi;
  std::vector<int> m_hitGasGap;
  std::vector<int> m_hitMeasPhi;
  std::vector<int> m_hitStrip;  
  std::vector<float> m_hitPos_x;  
  std::vector<float> m_hitPos_y;  
  std::vector<float> m_hitPos_z;  
  std::vector<float> m_hitTime;  

  // hits on track
  int m_nHitsOnTrack;
  std::vector<int> m_hitOnTrackStationName;
  std::vector<int> m_hitOnTrackStationEta;
  std::vector<int> m_hitOnTrackStationPhi;
  std::vector<int> m_hitOnTrackDoubletR;
  std::vector<int> m_hitOnTrackDoubletZ;
  std::vector<int> m_hitOnTrackDoubletPhi;
  std::vector<int> m_hitOnTrackGasGap;
  std::vector<int> m_hitOnTrackMeasPhi;
  std::vector<int> m_hitOnTrackStrip;

  // roi number
  int m_nRoi;
  std::vector<int> m_roiSide;
  std::vector<int> m_roiSector;
  std::vector<int> m_roiNumber;
  std::vector<int> m_roiEta;
  std::vector<int> m_roiPhi;

  // full PRD block
  int m_nPrd;
  std::vector<int>   m_prdStationName;
  std::vector<int>   m_prdStationEta;
  std::vector<int>   m_prdStationPhi;
  std::vector<int>   m_prdDoubletR;
  std::vector<int>   m_prdDoubletZ;
  std::vector<int>   m_prdDoubletPhi;
  std::vector<int>   m_prdGasGap;
  std::vector<int>   m_prdMeasPhi;
  std::vector<int>   m_prdStrip;
  std::vector<float> m_prdTime;
  std::vector<float> m_prdPos_x;
  std::vector<float> m_prdPos_y;
  std::vector<float> m_prdPos_z;

  // passed zero bias triggers
  int m_HLT_noalg_zb_L1ZB;
  int m_HLT_j40_L1ZB;
  int m_HLT_noalg_L1RD0_EMPTY;
  int m_HLT_noalg_L1RD0_FILLED;
};

#endif
