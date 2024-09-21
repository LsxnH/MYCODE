
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonRPC_Cabling/hardwareId.h"

#include "L1MuonHistoTool.h"

static const InterfaceID IID_L1MuonHistoTool("IID_L1MuonHistoTool", 1, 0);

const InterfaceID& L1MuonHistoTool::interfaceID() { return IID_L1MuonHistoTool; }

L1MuonHistoTool::L1MuonHistoTool(const std::string& type,
				       const std::string& name,
				       const IInterface* parent): 
  AthHistogramTool(type,name,parent)
{
  declareInterface<L1MuonHistoTool>(this);

}


L1MuonHistoTool::~L1MuonHistoTool()
{

}

StatusCode L1MuonHistoTool::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG("In initialize()");

  // retrieve the muon detector manager and the odhelper
  sc = detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER");
  if ( sc.isFailure() ) {
    msg() << MSG::ERROR << "Could not retrieve the RpcIdHelper" << endreq;
    return StatusCode::FAILURE;
  } 

  sc = bookHistos();
  if ( sc != StatusCode::SUCCESS ) {
    msg() << MSG::ERROR << "Could not book the histograms" << endreq;
    return StatusCode::FAILURE;
  }

  return sc;
}

StatusCode L1MuonHistoTool::finalize()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  
  return sc;
}


// book the efficiency histograms
StatusCode L1MuonHistoTool::bookHistos()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_MSG_DEBUG("Booking the histograms");

  int nDoubletR = 2;
  int nGasGap   = 2;
  int nMeasPhi  = 2;
  
  // indeces corresponding to the station types
  int nStationTypes = 8;
  std::string  stationTypes[8] = {"BML","BMS","BOL","BOS","BMF","BOF","BOG","BME"};
  //int  stationNamesInt[8] = {2,3,4,5,8,9,10,53};
  std::string measuresPhi[2] = {"etaStrips","phiStrips"};

  // retrieve the histograms
  for ( int istation = 0 ; istation < nStationTypes  ; istation++ ) {
    for ( int idoubletr = 0 ; idoubletr < nDoubletR  ; idoubletr++ ) {
      for ( int igasgap = 0 ; igasgap < nGasGap      ; igasgap++ ) {
        for ( int imeasphi = 0 ; imeasphi < nMeasPhi ; imeasphi++ ) {
 
          // build the histogram name
          std::string numHistName,denHistName,effHistName,drstring,ggstring;
          std::ostringstream dr,gg;

          dr << idoubletr+1;
          gg << igasgap+1;
          
          drstring = dr.str();
          ggstring = gg.str();
          
          numHistName = "num"+stationTypes[istation]+"EtaPhi_"+measuresPhi[imeasphi]+"_dR"+drstring+"_gg"+ggstring;
          denHistName = "den"+stationTypes[istation]+"EtaPhi_"+measuresPhi[imeasphi]+"_dR"+drstring+"_gg"+ggstring;
          effHistName = "eff"+stationTypes[istation]+"EtaPhi_"+measuresPhi[imeasphi]+"_dR"+drstring+"_gg"+ggstring;
	  
	  ATH_MSG_DEBUG("booking histos for: " << stationTypes[istation] << "EtaPhi_" << measuresPhi[imeasphi] 
			<< "_dR" << drstring << "_gg"+ggstring);
	  
	  // book the histos with the offline convention
          m_denEtaPhi[istation][idoubletr][igasgap][imeasphi] = 
            new TH2F(denHistName.c_str(),denHistName.c_str(),34,-8.,9.,32,1.,17.);
	  ATH_CHECK(histSvc()->regHist(("/THIST/"+denHistName).c_str(), m_denEtaPhi[istation][idoubletr][igasgap][imeasphi]));
	  
          m_numEtaPhi[istation][idoubletr][igasgap][imeasphi] = 
            new TH2F(numHistName.c_str(),numHistName.c_str(),34,-8.,9.,32,1.,17.);
	  ATH_CHECK(histSvc()->regHist(("/THIST/"+numHistName).c_str(), m_numEtaPhi[istation][idoubletr][igasgap][imeasphi]));
	  
	}
      }
    }
  } 



  return sc;
}

// fill the efficiency histograms
void L1MuonHistoTool::fillHistos(std::vector<Identifier>& intersections, std::vector<float>& residuals) {

  if ( intersections.size() != residuals.size() ) {
    ATH_MSG_ERROR("Intersections size different from the residuals size");
    return;
  }

  ATH_MSG_DEBUG("in fillhistos, size: " << intersections.size() );

  // indeces corresponding to the station types
  int nStationTypes = 8;
  std::string  stationTypes[8] = {"BML","BMS","BOL","BOS","BMF","BOF","BOG","BME"};
  int  stationNamesInt[8] = {2,3,4,5,8,9,10,53};
  std::string measuresPhi[2] = {"etaStrips","phiStrips"};
  
  for ( unsigned int i=0 ; i<intersections.size() ; ++i ) {
    // fill the denominator

    Identifier intersectionId = intersections.at(i);

    int stationName = m_rpcIdHelper->stationName(intersectionId);
    int stationEta = m_rpcIdHelper->stationEta(intersectionId);
    int stationPhi = m_rpcIdHelper->stationPhi(intersectionId);

    int doubletR = m_rpcIdHelper->doubletR(intersectionId);
    int doubletPhi = m_rpcIdHelper->doubletPhi(intersectionId);
    int doubletZ = m_rpcIdHelper->doubletZ(intersectionId);

    int gasGap  = m_rpcIdHelper->gasGap(intersectionId);
    int measPhi = m_rpcIdHelper->measuresPhi(intersectionId);


    ATH_MSG_DEBUG("Filling histos for: stationName: " << stationName << " eta: " << stationEta << " phi: " << stationPhi 
		  << " dR: " << doubletR << " dPhi: " << doubletPhi << " dZ: " << doubletZ 
		  << " gg: " << gasGap << " measPhi: " << measPhi );
    
    int stationIndex = -1;

    for ( int j=0 ; j<nStationTypes ;++j ) {
      if ( stationNamesInt[j]==stationName ) {
	stationIndex=j;
	break;
      }
    } 

    ATH_MSG_DEBUG("Station index: " << stationIndex );
    
    
    hardwareId m_hardwareId;
    m_hardwareId.setOfflineID(stationName, stationEta, stationPhi,
			      doubletR, doubletPhi, doubletZ, gasGap);
    int hw_etaIndex = m_hardwareId.m_HwStationNumber;
    if (stationTypes[stationIndex]=="BME") hw_etaIndex = 4;
    if (stationTypes[stationIndex]=="BMF" && doubletR==2) {
      if (hw_etaIndex==1 && doubletZ==2) hw_etaIndex += 1;
      if (hw_etaIndex>=2)                hw_etaIndex += 1;
    }
    if ( stationEta<0 ) hw_etaIndex *= -1;
    
    int hw_phiSector = m_hardwareId.m_HwSector;
    
    float shiftZ = 0.;
    float shiftPhi = 0.;
 
    doubletZ == 1 ? shiftZ = -0.25 : shiftZ = +0.25;
    if ( stationEta<0 ) shiftZ *= -1.;
    shiftZ += 0.5;
    
    doubletPhi == 1 ? shiftPhi = 0.25 : shiftPhi = 0.75;
    
    
    m_denEtaPhi[stationIndex][doubletR-1][gasGap-1][measPhi]->Fill(hw_etaIndex+shiftZ,
								   hw_phiSector+shiftPhi);
    
    
    float residualCut = 100.;
    //    if ( measPhi==0 && abs(stationEta)>4 && abs(stationEta)<7 ) {
    //      residualCut = 100.;
    //    } 
    
    // enlarge the cut for phi strips to take into account the apparent residual shift 
    // in some gaps
    if ( measPhi==1 ) {
      residualCut = 120.;
    }

    
    // cut on the residuals and fill the numerator
    if ( fabs(residuals.at(i)) < residualCut  ) {
      m_numEtaPhi[stationIndex][doubletR-1][gasGap-1][measPhi]->Fill(hw_etaIndex+shiftZ,
								     hw_phiSector+shiftPhi);
    }
    
  }
  

  return;
}

// save the efficiency histograms
void L1MuonHistoTool::writeHistos()
{



  return;
}
