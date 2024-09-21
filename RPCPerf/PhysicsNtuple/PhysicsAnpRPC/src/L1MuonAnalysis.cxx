
// Local
#include "L1MuonAnalysis.h"

// Athena/Gaudi
#include "MuonIdHelpers/RpcIdHelper.h"
#include "GaudiKernel/ITHistSvc.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"

// ROOT
#include "TLorentzVector.h"

// C/C++
#include <iostream>
#include <sstream>
#include <string>

#define MZ 91188.0

//========================================================================================================
L1MuonAnalysis::L1MuonAnalysis(const std::string& name, ISvcLocator* pSvcLocator) :
  AthHistogramAlgorithm( name, pSvcLocator ),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_trigDecisionTool("Trig::TrigDecisionTool/TrigDecisionTool")
{
  declareProperty("doTagAndProbe",           m_doTagAndProbe=true);
  declareProperty("doOrthogonal",            m_doOrthogonal=false);
  declareProperty("doZeroBias",              m_doZeroBias=false);
  declareProperty("doNtuple",                m_doNtuple=false);
  declareProperty("doHistos",                m_doHistos=false);
  declareProperty("doCosmics",               m_doCosmics=false);
  declareProperty("triggerChainGroupRegExp", m_triggerChainGroupRegExp);
  declareProperty("doFullPRD",               m_doFullPRD=false);
  declareProperty("MuonMatchTool",           m_matchTool);
  declareProperty("ExtrapolatorTool",        m_muonExtrapolator);
  declareProperty("HistoTool",               m_muonHistoTool);
}

//========================================================================================================
L1MuonAnalysis::~L1MuonAnalysis()
{
}

//========================================================================================================
StatusCode L1MuonAnalysis::initialize()
{
  if ( detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER").isFailure() ) {
    msg(MSG::ERROR) << "Could not retrieve the RpcIdHelper" << endreq;
    return StatusCode::FAILURE;
  } 

  ATH_CHECK( m_idHelperTool.retrieve() );

  // initialize the trigger matching tool
  ATH_CHECK(m_matchTool.retrieve());

  // initialize the muon extrapolator
  ATH_CHECK(m_muonExtrapolator.retrieve());

  // initialize the histogram tool
  ATH_CHECK(m_muonHistoTool.retrieve());

  // initialize the trigger decision tool and the chain group
  //  ATH_CHECK(m_trigDecisionTool.retrieve());
  m_chainGroup = m_trigDecisionTool->getChainGroup(m_triggerChainGroupRegExp.c_str());

  std::string cg1 = "HLT_noalg_zb_L1ZB";
  std::string cg2 = "HLT_j40_L1ZB";
  std::string cg3 = "HLT_noalg_L1RD0_EMPTY";

  m_chainGroupZeroBias    = m_trigDecisionTool->getChainGroup(cg1.c_str());
  m_chainGroupZeroBiasJ40 = m_trigDecisionTool->getChainGroup(cg2.c_str());
  m_chainGroupEmpty       = m_trigDecisionTool->getChainGroup(cg3.c_str());

  // book the efficiency histograms
  bookHistos();

  //  m_outFile->cd();
  ATH_CHECK( service("THistSvc", m_thistSvc) );
  ATH_CHECK(bookTree());

  // uncomment to dump to file the surfaces of the RPC panels
  // m_muonExtrapolator->dumpSurfaces();

  return StatusCode::SUCCESS;
}

//========================================================================================================
StatusCode L1MuonAnalysis::finalize()
{
  return StatusCode::SUCCESS;
}

//========================================================================================================
StatusCode L1MuonAnalysis::execute()
{
  StatusCode sc = StatusCode::SUCCESS;

  if ( m_doFullPRD ) {
    // retrieve the PRD container
    fillPRDContainer();
  }

  // check the LVL1 RoI
  checkMuonRoi();

  sc = selectGoodMuons();
  if ( sc != StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Could not select the good muons" << endreq;
    return sc;
  }
  msg() << MSG::DEBUG << "Number of selected Muons: " << m_preselMuons.size() << endreq;

  bool selected = false;

  if ( m_doCosmics ) {
    selectSingleMuons(selected);
    m_tree->Fill();
    return StatusCode::SUCCESS;
  } 

  if ( m_doTagAndProbe ) { 
    selectZmumu(selected);
  }

  // if the Z is selected skip the single muons selection
  if ( selected ) {
    m_tree->Fill();
    return StatusCode::SUCCESS;
  }

  selected = false;

  if ( m_doOrthogonal ) {
    // check if one of the orthogonal triggers is satisfied
    if ( checkOrthoTrigger() ) {    
      selectSingleMuons(selected);
    }    
  }

  if ( selected ) {
    m_tree->Fill();
    return StatusCode::SUCCESS;
  }

  selected = false;

  if ( m_doZeroBias ) {

    if ( checkZeroBiasTrigger() ) {
      //selectSingleMuons(selected);
      m_tree->Fill();
    }
  }
 
  return sc;
}

//========================================================================================================
void L1MuonAnalysis::selectZmumu(bool& selected)
{
  
  selected = false;
  // skip the event if less than two reconstructed muons
  if ( m_preselMuons.size()<2 ) {
    return;
  }

  selectMuonPairs();

  if ( m_muonPairs.size()==0 ) {
    return;
  }  

  msg() << MSG::DEBUG << "Best pair mass; " 
	<< ( m_bestPair.at(0)->p4() + m_bestPair.at(1)->p4() ).M() << endreq; 
  
  // deltaPhi cut
  //  double deltaPhi = m_bestPair.at(0)->phi() - m_bestPair.at(1)->phi();

  // check the match and analyse the othe muon
  if ( m_bestPair.at(0)->pt()>20000.0 &&  triggerMatch(m_bestPair.at(0)) ) {
    
    m_nTags++;
    const xAOD::Muon* tagMuon = m_bestPair.at(0);
    m_tagEta.push_back(tagMuon->eta());
    m_tagPhi.push_back(tagMuon->phi());
    m_tagPt.push_back(tagMuon->pt());

    const xAOD::Muon* mu = m_bestPair.at(1);
    if ( fabs(mu->eta())<1.05 ) { 

      analyseMuon(mu);
      m_nProbes++;

      m_probeEta.push_back(mu->eta());
      m_probePhi.push_back(mu->phi());
      m_probePt.push_back(mu->pt());
      
      checkTriggerMatch(mu);
    }
  }
  if ( m_bestPair.at(1)->pt()>20000. && triggerMatch(m_bestPair.at(1)) ) {

    m_nTags++;
    const xAOD::Muon* tagMuon = m_bestPair.at(1);
    m_tagEta.push_back(tagMuon->eta());
    m_tagPhi.push_back(tagMuon->phi());
    m_tagPt.push_back(tagMuon->pt());

    const xAOD::Muon* mu = m_bestPair.at(0);
    if ( fabs(mu->eta())<1.05 ) { 

      analyseMuon(mu);
      m_nProbes++;
      m_probeEta.push_back(mu->eta());
      m_probePhi.push_back(mu->phi());
      m_probePt.push_back(mu->pt());

      checkTriggerMatch(mu);
    }
  }

  if ( m_nProbes>0 ) {
    selected = true;
  }

  return;
}

//========================================================================================================
//
// function to select the single muons in orthogonal trigger
//
void L1MuonAnalysis::selectSingleMuons(bool& selected)
{

  ATH_MSG_DEBUG("In select single muons");

  selected = false;

  ATH_MSG_DEBUG("Orthogonal triggers passed ");

  // loop on the preselected muons and extrapolate them
  for ( unsigned int i=0 ; i<m_preselMuons.size() ; ++i ) {
    const xAOD::Muon* mu = m_preselMuons.at(i);

    // additional selection cuts on the probe muon
    if ( fabs(mu->eta())<1.05 ) {

      ATH_MSG_DEBUG("Foud a barrel muon with pT, eta, phi: " << mu->pt() << " " << mu->eta() << " " 
		    << mu->phi() );
      m_nProbes++; 
      analyseMuon(mu);
      m_probeEta.push_back(mu->eta());
      m_probePhi.push_back(mu->phi());
      m_probePt.push_back(mu->pt());
      
      // add the q/p relative error 
      if ( (mu->extrapolatedMuonSpectrometerTrackParticleLink()).isValid() ) { 
	const xAOD::TrackParticle* meTP = *mu->extrapolatedMuonSpectrometerTrackParticleLink();
	if ( meTP ) {
	  //	  std::cout << "pt: " << meTP->pt() << std::endl;
	  AmgVector(5) pars = meTP->definingParameters();
	  AmgSymMatrix(5) cov = meTP->definingParametersCovMatrix();
	  m_probeQOverPErr.push_back(sqrt(cov(4,4))/pars[4]);
	}
      }


      ATH_MSG_DEBUG("Now check the trigger match");

      checkTriggerMatch(mu);
    }

    
  } 

  if ( m_nProbes>0 ) {
    selected = true;
  }


  return;
}

//========================================================================================================
// do the analysis on a single muon
//
void L1MuonAnalysis::analyseMuon(const xAOD::Muon* muon)
{
  
  // get the track intersections with RPCs
  std::vector<Identifier> intersections;
  //  std::vector<Identifier> closestHit;
  std::vector<Muon::RpcPrepData> closestHit;
  std::vector<Amg::Vector3D> positions;
  std::vector<float> residuals;

  //  m_muonExtrapolator->extrapolateMuon(muon,intersections,positions,residuals);
  int result = m_muonExtrapolator->getIntersections(muon,intersections,positions,residuals,closestHit);

  if ( result<0 ) {
    msg() << MSG::WARNING << "Could not find intersections" << endreq;
    return;
  } 

  if ( intersections.size() != positions.size() )  {
    msg() << MSG::WARNING << "Identifiers and positions have different size" << endreq;
    return;
  }

  for ( unsigned int i=0 ; i<positions.size() ; ++i ) {
    m_nExpHits++;
    m_exphitpos_x.push_back(positions.at(i).x());
    m_exphitpos_y.push_back(positions.at(i).y());
    m_exphitpos_z.push_back(positions.at(i).z());

    Identifier id = intersections.at(i);
    m_stationName.push_back(m_rpcIdHelper->stationName(id));
    m_stationEta.push_back(m_rpcIdHelper->stationEta(id));
    m_stationPhi.push_back(m_rpcIdHelper->stationPhi(id));
    m_doubletR.push_back(m_rpcIdHelper->doubletR(id));
    m_doubletZ.push_back(m_rpcIdHelper->doubletZ(id));
    m_doubletPhi.push_back(m_rpcIdHelper->doubletPhi(id));
    m_gasGap.push_back(m_rpcIdHelper->gasGap(id));
    m_measPhi.push_back(m_rpcIdHelper->measuresPhi(id));
    m_strip.push_back(m_rpcIdHelper->strip(id));

    m_residuals.push_back(residuals.at(i));

    Muon::RpcPrepData rpcPrd = closestHit.at(i);
    Identifier hitId = rpcPrd.identify();
    // add the identifier of the closest hit
    m_hitStationName.push_back(m_rpcIdHelper->stationName(hitId));
    m_hitStationEta.push_back(m_rpcIdHelper->stationEta(hitId));
    m_hitStationPhi.push_back(m_rpcIdHelper->stationPhi(hitId));
    m_hitDoubletR.push_back(m_rpcIdHelper->doubletR(hitId));
    m_hitDoubletZ.push_back(m_rpcIdHelper->doubletZ(hitId));
    m_hitDoubletPhi.push_back(m_rpcIdHelper->doubletPhi(hitId));
    m_hitGasGap.push_back(m_rpcIdHelper->gasGap(hitId));
    m_hitMeasPhi.push_back(m_rpcIdHelper->measuresPhi(hitId));
    m_hitStrip.push_back(m_rpcIdHelper->strip(hitId));

    float xpos=0.0;
    float ypos=0.0;
    float zpos=0.0;

    if ( rpcPrd.detectorElement() ) {
      xpos = rpcPrd.globalPosition().x();
      ypos = rpcPrd.globalPosition().y();
      zpos = rpcPrd.globalPosition().z();      
    }

    m_hitPos_x.push_back(xpos);
    m_hitPos_y.push_back(ypos);
    m_hitPos_z.push_back(zpos);

    m_hitTime.push_back(rpcPrd.time());
  } 

  if ( m_doHistos ) {
    m_muonHistoTool->fillHistos(intersections,residuals);
  }

  // Add each muon's hits on track to the ntuple
  const Trk::Track* track = muon->primaryTrackParticle()->track();
  const DataVector<const Trk::MeasurementBase>* measurements = track->measurementsOnTrack();
  DataVector<const Trk::MeasurementBase>::const_iterator it = measurements->begin();
  for ( ; it != measurements->end() ; ++it ) {
    const Trk::MeasurementBase* meas = *it;
    const Trk::RIO_OnTrack* rot = dynamic_cast<const Trk::RIO_OnTrack*>(meas);
    
    if (!rot) continue;
    
    Identifier id_rot = rot->identify();
    // check if it's a RPC rot
    if ( m_idHelperTool->isRpc(id_rot) ) {
      
      m_nHitsOnTrack++;

      m_hitOnTrackStationName.push_back(m_rpcIdHelper->stationName(id_rot) );
      m_hitOnTrackStationEta.push_back( m_rpcIdHelper->stationEta(id_rot) );
      m_hitOnTrackStationPhi.push_back( m_rpcIdHelper->stationPhi(id_rot) );
      m_hitOnTrackDoubletR.push_back(m_rpcIdHelper->doubletR(id_rot));
      m_hitOnTrackDoubletZ.push_back(m_rpcIdHelper->doubletZ(id_rot));
      m_hitOnTrackDoubletPhi.push_back(m_rpcIdHelper->doubletPhi(id_rot));

      m_hitOnTrackGasGap.push_back(m_rpcIdHelper->gasGap(id_rot));
      m_hitOnTrackMeasPhi.push_back(m_rpcIdHelper->measuresPhi(id_rot));
      m_hitOnTrackStrip.push_back(m_rpcIdHelper->strip(id_rot));

    } 

  } 

  return;
}

//========================================================================================================
// fill the preprawdatacontainer
//
void L1MuonAnalysis::fillPRDContainer() 
{
  const Muon::RpcPrepDataContainer* rpcContainer;

  m_nPrd = 0;
  if ( evtStore()->retrieve(rpcContainer,"RPC_Measurements").isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve the RPC PrepRawData container " << endreq;
    return;
  } 

  Muon::RpcPrepDataContainer::const_iterator cont_it = rpcContainer->begin();

  // loop on the container
  for ( ; cont_it != rpcContainer->end() ; ++cont_it ) {

    const Muon::RpcPrepDataCollection* rpcCollection = (*cont_it);

    Muon::RpcPrepDataCollection::const_iterator coll_it = rpcCollection->begin();
    
    for ( ; coll_it != rpcCollection->end() ; ++coll_it ) {
      
      const Muon::RpcPrepData* rpcData = *coll_it;

      m_nPrd++;
      
      Identifier id = rpcData->identify();

      m_prdStationName.push_back(m_rpcIdHelper->stationName(id));
      m_prdStationEta.push_back(m_rpcIdHelper->stationEta(id));
      m_prdStationPhi.push_back(m_rpcIdHelper->stationPhi(id));

      m_prdDoubletR.push_back(m_rpcIdHelper->doubletR(id));
      m_prdDoubletZ.push_back(m_rpcIdHelper->doubletZ(id));
      m_prdDoubletPhi.push_back(m_rpcIdHelper->doubletPhi(id));
      m_prdGasGap.push_back(m_rpcIdHelper->gasGap(id));

      m_prdMeasPhi.push_back(m_rpcIdHelper->measuresPhi(id));
      m_prdStrip.push_back(m_rpcIdHelper->strip(id));

      // hit time and hit position
      m_prdTime.push_back(rpcData->time());

      // get the hit position
      const MuonGM::RpcReadoutElement* readoutEl = rpcData->detectorElement();
      Amg::Vector3D globalPos = readoutEl->stripPos(id);
      m_prdPos_x.push_back(globalPos.x());
      m_prdPos_y.push_back(globalPos.y());
      m_prdPos_z.push_back(globalPos.z());
      
    } 

  }
}

//========================================================================================================
// function to select the good muons for analysis
StatusCode L1MuonAnalysis::selectGoodMuons()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  m_preselMuons.clear();

  const xAOD::MuonContainer* muonContainer = 0;
  // retrieve the muon container
  sc = evtStore()->retrieve(muonContainer,"Muons");

  msg() << MSG::DEBUG << "Size of the muon container " << muonContainer->size() << endreq;

  xAOD::MuonContainer::const_iterator it = muonContainer->begin();
  for ( ; it != muonContainer->end() ; ++it ) {
    

    // skip segment tagged muons
    if ( ( (*it)->author()==xAOD::Muon::Author::MuTagIMO && (*it)->muonType()==xAOD::Muon::MuonType::SegmentTagged ) 
	 || (!(*it)->primaryTrackParticle()) ) continue;

    uint8_t numPrecLayers;
    float momBalSig = 0.0;
    (*it)->summaryValue(numPrecLayers, xAOD::SummaryType::numberOfPrecisionLayers );
    (*it)->parameter(momBalSig,xAOD::Muon::momentumBalanceSignificance);

    float reducedChi2;
    if ( (*it)->primaryTrackParticle() ) {
      reducedChi2 = (*it)->primaryTrackParticle()->chiSquared()/
	(*it)->primaryTrackParticle()->numberDoF();
    }
    else {
      reducedChi2 = 1000.0;
    }    

    if ( !m_doCosmics ) {
      if ( ((*it)->quality()==xAOD::Muon::Medium ||          
	    (*it)->quality()==xAOD::Muon::Tight ) &&
	   (*it)->muonType()==xAOD::Muon::Combined &&
	   (*it)->pt()>2000.0 &&
	   fabs((*it)->primaryTrackParticle()->d0())<20 &&
	   fabs((*it)->primaryTrackParticle()->z0())<100 && 
	   fabs(momBalSig)< 3.0 && 
	   numPrecLayers>1 && 
	   reducedChi2 < 8  ) 
	{    
	  m_preselMuons.push_back(*it);
	}
    }
    else {
      m_preselMuons.push_back(*it);
    }

  }
  
  
  return sc;
}

//========================================================================================================
// build the muon pairs
void L1MuonAnalysis::selectMuonPairs() 
{

  m_muonPairs.clear();

  float massDiff = 999999.;

  for ( unsigned int i=0 ; i<m_preselMuons.size() ; ++i ) {
    const xAOD::Muon* mu1 = m_preselMuons.at(i);

    for ( unsigned int j=i+1 ; j<m_preselMuons.size() ; ++j ) {
      
      const xAOD::Muon* mu2 = m_preselMuons.at(j);
     
      if ( mu1->charge() * mu2->charge()<0. ) {

	// cut on delta phi
	double phi1 = mu1->phi();
	double phi2 = mu2->phi();
	if ( phi1<0 ) phi1 += 2.*CLHEP::pi;  
	if ( phi2<0 ) phi2 += 2.*CLHEP::pi;  
	double deltaPhi = abs(phi1-phi2);
	if ( deltaPhi > 2. ) {
	  
	  float pairMass = ( mu1->p4() + mu2->p4() ).M();
	  // cut on the di-muon mass
	  if ( abs(pairMass-MZ)<15000.0 ) { 
	    
	    std::vector<const xAOD::Muon*> thisPair;
	    thisPair.push_back(mu1);
	    thisPair.push_back(mu2);
	    m_muonPairs.push_back(thisPair);
	    
	    if ( abs(pairMass-MZ)<massDiff ) {
	      massDiff = abs(pairMass-MZ);
	      m_bestPair = thisPair;
	      m_diMuonMass = pairMass;
	      
	    }


	  }  
	}

      } 
    }

  } 
  
}

//========================================================================================================
// set the flags of the zero bias triggers
//
bool L1MuonAnalysis::checkZeroBiasTrigger()
{
  
  bool passed = false;

  if ( m_chainGroupZeroBias->isPassed() ) {
    m_HLT_noalg_zb_L1ZB = 1;
    passed = true;
  } 
  if ( m_chainGroupZeroBiasJ40->isPassed() ) {
    m_HLT_j40_L1ZB = 1;
    passed = true;
  } 
  if ( m_chainGroupEmpty->isPassed() ) {
    m_HLT_noalg_L1RD0_EMPTY = 1;
    passed = true;
  } 

  return passed;
}

//========================================================================================================
// check if the orthogonal triggers have passed
//
bool L1MuonAnalysis::checkOrthoTrigger()
{
  
  bool passed = false;
 
  passed = m_chainGroup->isPassed();

  if ( passed ) {
    ATH_MSG_DEBUG("Passed trigger chain group: " << m_triggerChainGroupRegExp );
  }
  else {
    ATH_MSG_DEBUG("Trigger chain group not passed ");    
  }

  return passed;
}

//========================================================================================================
// check the match with the 6 trigger thresholds
//
void L1MuonAnalysis::checkTriggerMatch(const xAOD::Muon* mu) 
{
  bool match[6];

  match[0] = m_matchTool->matchL1(mu,"L1_MU4",0.2);
  match[1] = m_matchTool->matchL1(mu,"L1_MU6",0.2);
  match[2] = m_matchTool->matchL1(mu,"L1_MU10",0.2);
  match[3] = m_matchTool->matchL1(mu,"L1_MU11",0.2);
  match[4] = m_matchTool->matchL1(mu,"L1_MU15",0.2);
  match[5] = m_matchTool->matchL1(mu,"L1_MU20",0.2);

  if (match[0]) m_matchTh1.push_back(1); 
  else m_matchTh1.push_back(0);
  if (match[1]) m_matchTh2.push_back(1); 
  else m_matchTh2.push_back(0);
  if (match[2]) m_matchTh3.push_back(1); 
  else m_matchTh3.push_back(0);
  if (match[3]) m_matchTh4.push_back(1); 
  else m_matchTh4.push_back(0);
  if (match[4]) m_matchTh5.push_back(1); 
  else m_matchTh5.push_back(0);
  if (match[5]) m_matchTh6.push_back(1); 
  else m_matchTh6.push_back(0);

}

//========================================================================================================
// match a reconstructed muon to the trigger RoI
//
bool L1MuonAnalysis::triggerMatch(const xAOD::Muon* mu)
{
  
  bool match = m_matchTool->matchL1(mu,"L1_MU20",0.2);

  if (match) {
    ATH_MSG_DEBUG("Found a tag match with L1_MU20");
  }
  else {
    ATH_MSG_DEBUG("Match with L1_MU20 not found");
  }

  return match;
}

//========================================================================================================
// check the content of the roi
void L1MuonAnalysis::checkMuonRoi() 
{

  const xAOD::MuonRoIContainer* muonRoIs;

  // retrieve the container of the muon RoIs
  StatusCode sc = evtStore()->retrieve(muonRoIs,"LVL1MuonRoIs");
  if ( sc.isFailure() ) {
    ATH_MSG_ERROR("Failed to access the LVL1MuonRoIs in StoreGate");
    return;
  } 

  // loop on the roi container
  xAOD::MuonRoIContainer::const_iterator it = muonRoIs->begin();
  for  ( ; it !=muonRoIs->end() ; ++it ) {
    
    if ( (*it)->getSource() == xAOD::MuonRoI::RoISource::Barrel ) { 
      
      m_nRoi++;
      // get the roi word and decode it
      unsigned int roiWord = (*it)->roiWord();
      unsigned int sectorAddress = (roiWord & 0x003FC000) >> 14;
      unsigned int sectorRoIOvl  = (roiWord & 0x000007FC) >> 2;
      unsigned int side =  sectorAddress & 0x00000001;
      unsigned int sector = (sectorAddress & 0x0000003e) >> 1;
      unsigned int roiNumber =  sectorRoIOvl & 0x0000001F;      

      m_roiSide.push_back(side);
      m_roiSector.push_back(sector);
      m_roiNumber.push_back(roiNumber);
      m_roiEta.push_back((*it)->eta());
      m_roiPhi.push_back((*it)->phi());
      
    }

  }

  return;
}

//========================================================================================================
// Book histos and the tree
void L1MuonAnalysis::bookHistos()
{


  return;
}

//========================================================================================================
// book the ntuple
StatusCode L1MuonAnalysis::bookTree() 
{

  //  m_outFile->cd();
  
  m_tree = new TTree("rpc","rpc");
  
  ATH_CHECK(histSvc()->regTree("/TTREE/rpc",m_tree));

  m_tree->Branch("nProbes",&m_nProbes);
  m_tree->Branch("nTags",  &m_nTags);
  
  m_tree->Branch("diMuonMass",&m_diMuonMass);

  m_tree->Branch("probeEta",&m_probeEta);
  m_tree->Branch("probePhi",&m_probePhi);
  m_tree->Branch("probePt",&m_probePt);
  m_tree->Branch("probeQOverPErr",&m_probeQOverPErr);

  m_tree->Branch("tagEta",&m_tagEta);
  m_tree->Branch("tagPhi",&m_tagPhi);
  m_tree->Branch("tagPt", &m_tagPt);
  
  m_tree->Branch("matchTh1", &m_matchTh1);
  m_tree->Branch("matchTh2", &m_matchTh2);
  m_tree->Branch("matchTh3", &m_matchTh3);
  m_tree->Branch("matchTh4", &m_matchTh4);
  m_tree->Branch("matchTh5", &m_matchTh5);
  m_tree->Branch("matchTh6", &m_matchTh6);
  
  // intersections
  m_tree->Branch("nExpHits",&m_nExpHits);
  m_tree->Branch("exphitpos_x",&m_exphitpos_x);
  m_tree->Branch("exphitpos_y",&m_exphitpos_y);
  m_tree->Branch("exphitpos_z",&m_exphitpos_z);
  
  // residuals
  m_tree->Branch("stationName",&m_stationName);
  m_tree->Branch("stationEta", &m_stationEta);
  m_tree->Branch("stationPhi", &m_stationPhi);
  m_tree->Branch("doubletR",   &m_doubletR);
  m_tree->Branch("doubletZ",   &m_doubletZ);
  m_tree->Branch("doubletPhi", &m_doubletPhi);
  m_tree->Branch("gasGap",     &m_gasGap);
  m_tree->Branch("measPhi",    &m_measPhi);
  m_tree->Branch("strip",      &m_strip);
  m_tree->Branch("residuals",&m_residuals);
  
  m_tree->Branch("hitStationName",&m_hitStationName);
  m_tree->Branch("hitStationEta",&m_hitStationEta);
  m_tree->Branch("hitStationPhi",&m_hitStationPhi);
  m_tree->Branch("hitDoubletR",&m_hitDoubletR);
  m_tree->Branch("hitDoubletZ",&m_hitDoubletZ);
  m_tree->Branch("hitDoubletPhi",&m_hitDoubletPhi);
  m_tree->Branch("hitGasGap",&m_hitGasGap);
  m_tree->Branch("hitMeasPhi",&m_hitMeasPhi);
  m_tree->Branch("hitStrip",&m_hitStrip);
  m_tree->Branch("hitPos_x",&m_hitPos_x);
  m_tree->Branch("hitPos_y",&m_hitPos_y);
  m_tree->Branch("hitPos_z",&m_hitPos_z);
  m_tree->Branch("hitTime",&m_hitTime);
  
  // hits on track
  m_tree->Branch("nHitsOnTrack",&m_nHitsOnTrack);
  m_tree->Branch("hitOnTrackStationName",&m_hitOnTrackStationName);
  m_tree->Branch("hitOnTrackStationEta",&m_hitOnTrackStationEta);
  m_tree->Branch("hitOnTrackStationPhi",&m_hitOnTrackStationPhi);
  m_tree->Branch("hitOnTrackDoubletR",&m_hitOnTrackDoubletR);
  m_tree->Branch("hitOnTrackDoubletZ",&m_hitOnTrackDoubletZ);
  m_tree->Branch("hitOnTrackDoubletPhi",&m_hitOnTrackDoubletPhi);
  m_tree->Branch("hitOnTrackGasGap",&m_hitOnTrackGasGap);
  m_tree->Branch("hitOnTrackMeasPhi",&m_hitOnTrackMeasPhi);
  m_tree->Branch("hitOnTrackStrip",&m_hitOnTrackStrip);
  
  // info on the barrel RoIs
  m_tree->Branch("nRoi",&m_nRoi);
  m_tree->Branch("roiSide",&m_roiSide);
  m_tree->Branch("roiSector",&m_roiSector);
  m_tree->Branch("roiNumber",&m_roiNumber);
  m_tree->Branch("roiEta",&m_roiEta);
  m_tree->Branch("roiPhi",&m_roiPhi);
  
  if ( m_doFullPRD ) {
    
    m_tree->Branch("nPrd",          &m_nPrd);
    m_tree->Branch("prdStationName",&m_prdStationName);
    m_tree->Branch("prdStationEta", &m_prdStationEta);
    m_tree->Branch("prdStationPhi", &m_prdStationPhi);
    m_tree->Branch("prdDoubletR",   &m_prdDoubletR);
    m_tree->Branch("prdDoubletPhi", &m_prdDoubletPhi);
    m_tree->Branch("prdDoubletZ",   &m_prdDoubletZ);
    m_tree->Branch("prdMeasPhi",    &m_prdMeasPhi);
    m_tree->Branch("prdGasGap",     &m_prdGasGap);
    m_tree->Branch("prdStrip",      &m_prdStrip);
    m_tree->Branch("prdTime",       &m_prdTime);
    m_tree->Branch("prdPos_x",      &m_prdPos_x);
    m_tree->Branch("prdPos_y",      &m_prdPos_y);
    m_tree->Branch("prdPos_z",      &m_prdPos_z);
    
  } 
  
  
  // zero bias and lumi part
  m_tree->Branch("HLT_noalg_zb_L1ZB",&m_HLT_noalg_zb_L1ZB);
  m_tree->Branch("HLT_j40_L1ZB",&m_HLT_j40_L1ZB);
  m_tree->Branch("HLT_noalg_L1RD0_EMPTY",&m_HLT_noalg_L1RD0_EMPTY);
  //  m_tree->Branch("HLT_noalg_L1RD0_FILLED",&m_HLT_noalg_L1RD0_FILLED);
  
  return StatusCode::SUCCESS;
}
