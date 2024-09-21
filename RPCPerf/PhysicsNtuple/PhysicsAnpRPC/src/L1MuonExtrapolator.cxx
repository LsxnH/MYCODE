#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonIdHelpers/RpcIdHelper.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonReadoutGeometry/RpcReadoutElement.h"
#include "MuonReadoutGeometry/RpcDetectorElement.h"
#include "MuonReadoutGeometry/RpcReadoutSet.h"
#include "TrkExInterfaces/IExtrapolator.h"

#include "MuonTGRecTools/MuonTGMeasurementTool.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "GeoPrimitives/GeoPrimitivesHelpers.h"

#include "L1MuonExtrapolator.h"

#include <math.h>
#include <fstream>

static const InterfaceID IID_L1MuonExtrapolator("IID_L1MuonExtrapolator", 1, 0);

const InterfaceID& L1MuonExtrapolator::interfaceID() { return IID_L1MuonExtrapolator; }

L1MuonExtrapolator::L1MuonExtrapolator(const std::string& type,
				       const std::string& name,
				       const IInterface* parent): 
  AthAlgTool(type,name,parent),
  m_idHelperTool("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_extrapolator("Trk::Extrapolator/AtlasExtrapolator")
{
  declareInterface<L1MuonExtrapolator>(this);

}


L1MuonExtrapolator::~L1MuonExtrapolator()
{

}

StatusCode L1MuonExtrapolator::initialize()
{
  StatusCode sc = StatusCode::SUCCESS;

  ATH_CHECK( detStore()->retrieve(m_muonMgr) );
  ATH_CHECK( detStore()->retrieve(m_rpcIdHelper,"RPCIDHELPER") );
  ATH_CHECK( m_idHelperTool.retrieve() );
  ATH_CHECK( m_extrapolator.retrieve() );

  // Locate RegionSelector                                    
//  sc = service("RegSelSvc", m_regionSelector); 
//  if(sc.isFailure()) {                                  
//    msg() << MSG::ERROR << "Could not retrieve RegionSelector" << endreq;            
//    return sc;             
//  } 
//  msg() << MSG::DEBUG << "Retrieved service RegionSelector" << endreq;  

  return sc;
}

StatusCode L1MuonExtrapolator::finalize()
{
  StatusCode sc = StatusCode::SUCCESS;
  
  
  return sc;
}

// get the track state on surface closest to a given detector element
const Trk::TrackStateOnSurface* L1MuonExtrapolator::getClosestSurface(const xAOD::TrackParticle* tp, 
								      const MuonGM::RpcDetectorElement* detEl)
{

  if ( !tp ) {
    msg()<< MSG::WARNING << "Empty track particle, skipping extrapolation " << endreq;
    return NULL;
  }
  const Trk::Track* track = tp->track();
  const Trk::TrackStateOnSurface* tsos = NULL;
  double minDistance = 999999.;
  if (track) {
    const DataVector<const Trk::TrackStateOnSurface>* trackStateOnSurfaces =  track->trackStateOnSurfaces();
    if ( trackStateOnSurfaces ) {

      // look for the one closest to the detector elelement
      const Trk::Surface& detSurface = detEl->surface();
      Amg::Vector3D detCenter(detSurface.center().x(),detSurface.center().y(),detSurface.center().z());
      
      DataVector<const Trk::TrackStateOnSurface>::const_iterator  it = trackStateOnSurfaces->begin();
      msg() << MSG::DEBUG << "Number of crossed surfaces: " << trackStateOnSurfaces->size() << endreq;
      
      for ( ; it != trackStateOnSurfaces->end() ; ++it ) {
	const Trk::TrackParameters* par=(*it)->trackParameters();

	if ( !par ) {
	  msg()<< MSG::WARNING << "Track parameters not found, skipping the surface " << endreq;
	  continue;
	} 
	Amg::Vector3D surfPosition = par->position();
	float distance = Amg::distance(detCenter,surfPosition);
	if (abs(distance)<minDistance) {
	  minDistance = abs(distance);
	  tsos = (*it);
	}
      }
    }
    else {
      msg() << MSG::WARNING << "Track states on surfaces not found" << endreq;
      return tsos;
    }
  }
  else {
    msg() << MSG::WARNING << "Track associated to muon not found" << endreq;
    return tsos;
  }
  msg() << MSG::DEBUG << ">>> Minimal distance found: " << minDistance << endreq;

  return tsos;
}

// get the correction to the phi residuals, according to the phi station sector
float L1MuonExtrapolator::getPhiCorrection(float phiIntersection, float centralPhiStrip, float centralPhiR)
{
  float phiCorrection = 0.0;

  float deltaPhi = phiIntersection-centralPhiStrip;
  phiCorrection = centralPhiR*(tan(deltaPhi)-sin(deltaPhi));

  return phiCorrection;
}


// get the intersections of the muon with the RPC planes
int L1MuonExtrapolator::getIntersections(const xAOD::Muon* muon, 
					 std::vector<Identifier>& intersections,
					 std::vector<Amg::Vector3D>& positions, 
					 std::vector<float>& residuals,
					 std::vector<Muon::RpcPrepData>& closestHit)
{

  if ( !muon ) {
    msg() << MSG::WARNING << "Muon pointer not initialized" << endreq;
    return -1;
  }

  const xAOD::TrackParticle* tp = muon->primaryTrackParticle();

  if (!tp) {
    msg() << MSG::WARNING << "Could not find the track particle associated to the muon. Author, type: " 
	  << muon->author() << " " << muon->muonType() << endreq; 
    return -1;
  }

  msg() << MSG::DEBUG << ">>>>>>>>>>>>>>> New MUON to analyse" << endreq;
  
  msg() << MSG::DEBUG << "pT: " << tp->pt() << "  eta: " << tp->eta() << "  phi: " << tp->phi() << endreq; 

  // get the list of detector elements to compute the intersections
  double etaMin = tp->eta()-0.5;
  double etaMax = tp->eta()+0.5;
  double phi    = tp->phi();
  double phiMin = tp->phi()-0.5;
  double phiMax = tp->phi()+0.5;
  if( phi < 0 ) phi += 2*CLHEP::pi;
  //  if( phiMin < 0 ) phiMin += 2*CLHEP::pi;
  //  if( phiMax < 0 ) phiMax += 2*CLHEP::pi;

  msg() << MSG::DEBUG << "etaMin, etaMax: " << etaMin << "," << etaMax << "  phiMin,phiMax: " 
	<< phiMin << "," << phiMax << endreq;

//A  TrigRoiDescriptor* roi = new TrigRoiDescriptor(tp->eta(),etaMin,etaMax,phi,phiMin,phiMax);
//A  //  const IRoiDescriptor* iroi = (IRoiDescriptor*) roi;
  std::vector<IdentifierHash> rpcHashList;
//A  m_regionSelector->DetHashIDList(RPC, *roi, rpcHashList);

 
  //  std::cout << "AAAAA >>> size of the hashId: " << rpcHashList.size() << std::endl;

  // look for RPC hits in the same detector element
  const Muon::RpcPrepDataContainer* rpcContainer;
  StatusCode sc = evtStore()->retrieve(rpcContainer,"RPC_Measurements");
  if (sc.isFailure()) {
    msg() << MSG::WARNING << "Could not retrieve the RPC PRD container" << endreq;
    return -1;
  }
  
  msg() << MSG::DEBUG << "The size of the hashID list is: " << rpcContainer->fullSize() << endreq;
    // retrieve the detector elements
  
  for ( int index=0 ; index<rpcContainer->size() ; ++index ) {
    rpcHashList.push_back(index);
  } 


  if (!rpcHashList.empty()) {
    
    
    // loop on the collections and find the intersections
    // of the track with the surfaces
    // loop on the identifier Hash
    for ( std::vector<IdentifierHash>::const_iterator hash_it = rpcHashList.begin();
    	  hash_it != rpcHashList.end() ; ++hash_it ) {

      const MuonGM::RpcDetectorElement* detEl = m_muonMgr->getRpcDetectorElement((*hash_it));
      if (!detEl) {
	msg() << MSG::WARNING << "Could not retrieve the detector element" << endreq;
	return -1;
      }

      // check if the detector element is in the RoI 
      const Amg::Vector3D detElementCenter = detEl->center();
      double detEta = detElementCenter.eta();
      double detPhi = detElementCenter.phi();
      if( detPhi < 0 ) detPhi += 2*CLHEP::pi;

      double deltaPhi = abs(detPhi-phi);
      if ( deltaPhi>CLHEP::pi ) deltaPhi = 2*CLHEP::pi - deltaPhi;

      if ( !( detEta>etaMin && detEta<etaMax && deltaPhi<0.6 ) ) continue; 


      if ( msg().level()==MSG::DEBUG ) {
	msg() << MSG::DEBUG << " " << endreq;
	msg() << MSG::DEBUG << ">>>>>> CHECKING a new detector element: " << endreq;
	m_rpcIdHelper->print(detEl->identify());
      }

      // derive the best point to start the extrapolation from
      // get from the track states on surface the closest one to the detector element
      const Trk::TrackStateOnSurface* bestTsos = getClosestSurface(tp,detEl);
      if ( !bestTsos ) {
	msg() << MSG::WARNING << "Best track state on surface not found !" << endreq;
	return -1;
      }

      const Trk::TrackParameters* startParameters = bestTsos->trackParameters();
      const Trk::Surface& detSurface = detEl->surface();
      
      msg() << MSG::DEBUG << "CENTER of the detector element surface: " << detSurface.center().x() << " " 
      	    << detSurface.center().y() << " " << detSurface.center().z() << endreq; 
      //

      const Trk::TrackParameters* destParameters = m_extrapolator->extrapolate(*startParameters,
									       detSurface,
									       Trk::alongMomentum,
									       false,
									       (Trk::ParticleHypothesis) 2);

      
      const Amg::Vector3D& intersection = destParameters->position();
      if ( destParameters ) {

	const Amg::Vector3D& intersection = destParameters->position();
	
	if ( msg().level()==MSG::DEBUG ) {
	  std::cout << *destParameters << std::endl;
	}

	msg() << MSG::DEBUG << ">>>>>> Detector SURFACE INTERSECTION" << endreq;
	msg() << MSG::DEBUG << "Intersection position: " << intersection.x() << " "
	      << intersection.y() << " " << intersection.z() << "  phi: " << intersection.phi() 
	      << " R: " << intersection.perp() << endreq;

      } 
      else {
	msg() << MSG::DEBUG << "Extrapolation to the detector surface not done" << endreq;
	continue;
      }


      Identifier collId;
      IdContext rpcContext = m_rpcIdHelper->module_context(); 
      int getId = m_rpcIdHelper->get_id( (*hash_it) ,collId, &rpcContext);
      if ( getId != 0 ) {
	msg() << MSG::WARNING << "Could not convert the hashId to identifier" << endreq; 
	return -1;
      }
      else {
	//	std::cout << "Converting The hashId: " << (*hash_it) << std::endl;
	//	m_rpcIdHelper->print(collId);
      }


      // Now check if the intersection is within the acceptance of the detector element
      // detector element identifier fields
      // and find the closest strip
      //      int stationEta  = m_rpcIdHelper->stationEta(collId);
      int doubletR    = m_rpcIdHelper->doubletR(collId);
     
      // take a detector element to calculate the DR
      //      double sign = stationEta < 0 ? -1.0 : 1.0;

      //      int bestDoubletZ   = -1;
      //      int bestDoubletPhi = -1;
      int bestStripEta   = -1;
      int bestStripPhi   = -1;
      Amg::Vector3D extrEta1(0.,0.,0.);
      Amg::Vector3D extrEta2(0.,0.,0.);
      Amg::Vector3D extrPhi1(0.,0.,0.);
      Amg::Vector3D extrPhi2(0.,0.,0.);

      // loop on the doubletZ and the doubletPhi
      for ( int idz = 1 ; idz<3 ; ++idz ) {
	for ( int idp= 1 ; idp<3 ; ++idp ) { 
	  
	  bool inEtaAcceptance = false;
	  bool inPhiAcceptance = false;

	  const MuonGM::RpcReadoutElement* readoutEl = detEl->getRpcReadoutElement(idz,idp);
	  
	  if ( !readoutEl ) continue; 
	  
	  // loop on the eta strips to find the closest one
	  int neta = readoutEl->NetaStrips();
	  int nphi = readoutEl->NphiStrips();
	  //	  double phiPitch = readoutEl->StripPitch(1);

	  msg() << MSG::DEBUG << "New doublet! doubletZ: " << idz << " doubletPhi: " << idp 
		<< " neta strips: " << neta << " nphi strips: " << nphi << endreq;
	  
	  // first and last strip eta position (check the first gas gap)
	  Amg::Vector3D firstStripEtaPos = readoutEl->stripPos(doubletR,idz,idp,1,0,1);
	  Amg::Vector3D lastStripEtaPos  = readoutEl->stripPos(doubletR,idz,idp,1,0,neta);

	  msg() << MSG::DEBUG << "First strip eta pos: " << firstStripEtaPos.x() << " "  
		<< " " << firstStripEtaPos.y() << " " << firstStripEtaPos.z() << endreq;
	  msg() << MSG::DEBUG << "Last  strip eta pos: " << lastStripEtaPos.x() << " "  
		<< " " << lastStripEtaPos.y() << " " << lastStripEtaPos.z() << endreq;

	  // extrapolate the track to the first and last strip
	  Trk::PerigeeSurface firstStripEtaSurf(firstStripEtaPos);
	  Trk::PerigeeSurface lastStripEtaSurf(firstStripEtaPos);
	  const Trk::TrackParameters* firstStripEtaPerigee = 0;
	  const Trk::TrackParameters* lastStripEtaPerigee = 0;
	  firstStripEtaPerigee = m_extrapolator->extrapolateDirectly(*startParameters, firstStripEtaSurf);
	  lastStripEtaPerigee = m_extrapolator->extrapolateDirectly(*startParameters, lastStripEtaSurf);

	  if ( firstStripEtaPerigee && lastStripEtaPerigee) {
	    extrEta1 = firstStripEtaPerigee->position();
	    extrEta2 = lastStripEtaPerigee->position();
	    msg() << MSG::DEBUG << "First strip eta extrapolation: " << extrEta1.x() << " " 
		  << extrEta1.y() << " " << extrEta1.z() << endreq;
	    msg() << MSG::DEBUG << "Last strip eta extrapolation: " << extrEta2.x() << " " 
		  << extrEta2.y() << " " << extrEta2.z() << endreq;
	  }
	  else {
	    msg() << MSG::DEBUG << "No extrapolation done, skipping" << endreq;
	    continue;
	  }

	  
	  // check the eta acceptance
	  if ( fabs(intersection.z()) > (fabs(firstStripEtaPos.z())) 
	       && fabs(intersection.z()) < (fabs(lastStripEtaPos.z())) ) {
	    inEtaAcceptance = true;
	    msg() << MSG::DEBUG << "IN THE ETA ACCEPTANCE !!!" << endreq;
	    
	  } // check that the eta intersection is within the detector element acceptance

	  if ( !inEtaAcceptance ) continue; 
	    
	  // first and last strip eta position (check the first gas gap)
	  Amg::Vector3D firstStripPhiPos = readoutEl->stripPos(doubletR,idz,idp,1,1,1);
	  Amg::Vector3D lastStripPhiPos  = readoutEl->stripPos(doubletR,idz,idp,1,1,nphi);

	  // extrapolate the track to the first and last strip
	  Trk::PerigeeSurface firstStripPhiSurf(firstStripPhiPos);
	  Trk::PerigeeSurface lastStripPhiSurf(firstStripPhiPos);
	  const Trk::TrackParameters* firstStripPhiPerigee = 0;
	  const Trk::TrackParameters* lastStripPhiPerigee = 0;
	  firstStripPhiPerigee = m_extrapolator->extrapolateDirectly(*startParameters, firstStripPhiSurf);
	  lastStripPhiPerigee = m_extrapolator->extrapolateDirectly(*startParameters, lastStripPhiSurf);

	  msg() << MSG::DEBUG << "First strip phi pos: " << firstStripPhiPos.x() << " "  
		<< " " << firstStripPhiPos.y() << " " << firstStripPhiPos.z() 
		<< " phi: " << firstStripPhiPos.phi() 
		<< " R: " << firstStripPhiPos.perp() << endreq;
	  msg() << MSG::DEBUG << "Last  strip phi pos: " << lastStripPhiPos.x() << " "  
		<< " " << lastStripPhiPos.y() << " " << lastStripPhiPos.z() 
		<< " phi: " << lastStripPhiPos.phi() << " R: " << lastStripPhiPos.perp() << endreq;

	  if ( firstStripPhiPerigee && lastStripPhiPerigee) {
	    extrPhi1 = firstStripPhiPerigee->position();
	    extrPhi2 = lastStripPhiPerigee->position();
	    msg() << MSG::DEBUG << "First strip phi extrapolation: " << extrPhi1.x() << " " 
		  << extrPhi1.y() << " " << extrPhi1.z() << " " << extrPhi1.phi() << endreq;
	    msg() << MSG::DEBUG << "Last strip phi extrapolation: " << extrPhi2.x() << " " 
		  << extrPhi2.y() << " " << extrPhi2.z() << " " << extrPhi2.phi() << endreq;
	  }
	  else {
	    msg() << MSG::DEBUG << "No extrapolation done, skipping" << endreq;
	    continue;
	  }

	  double phiIntersection = intersection.phi();
	  double phiFirst = firstStripPhiPos.phi();
	  double phiLast  = lastStripPhiPos.phi();
	  if( phiIntersection < 0 ) phiIntersection += 2*CLHEP::pi;
	  if( phiFirst < 0 ) phiFirst += 2*CLHEP::pi;
	  if( phiLast < 0 )  phiLast += 2*CLHEP::pi;
	  
	  msg() << MSG::DEBUG << "Corrected phi intersection, phiFirst, phiLast: " << phiIntersection << " " 
		<< phiFirst << " " << phiLast << endreq;

	  if ( phiLast<phiFirst ) {
	    double phiTemp = phiLast;
	    phiLast = phiFirst;
	    phiFirst = phiTemp;
	  } 
	  if ( phiIntersection > phiFirst && phiIntersection < phiLast ) { 
	    inPhiAcceptance = true;
	    msg() << MSG::DEBUG << "IN THE PHI ACCEPTANCE !!!" << endreq;	    
	  }  // check that the intersection is in the phi range of the detel
	    
	   	    
	  // if it's in the acceptance, find the closest strips and try to get the corresponding hits
	  if ( inEtaAcceptance && inPhiAcceptance ) {

	    msg() << MSG::DEBUG << ">>> The muon is in the acceptance !!! " << endreq;
            
	    Identifier closestId[2][2];
	    Muon::RpcPrepData closestHitArray[2][2];
	    float residualsArray[2][2] = {999999., 999999., 999999., 999999.};
	    
	    // find the closest strip
	    double bestEtaDist = 99999.;	    
	    double bestPhiDist = 99999.;
	    // find the best eta strips in the two gas gaps
	    //bestDoubletZ = idz;
	    // look for the best match strip
	    for ( int istripeta = 1 ; istripeta<neta+1 ; ++istripeta) {
	      Amg::Vector3D stripEtaPos = readoutEl->stripPos(doubletR,idz,idp,1,0,istripeta);
	      // here extrapolate to each strip

	      if ( fabs(intersection.z()-stripEtaPos.z() ) < bestEtaDist ) {
		bestEtaDist = fabs(extrEta1.z()-stripEtaPos.z());
		bestStripEta = istripeta;
	      } 	      
	    } // loop on the eta strips

	    Identifier etaStripId_gap1 = m_rpcIdHelper->channelID(collId,idz,idp,1,0,bestStripEta);
	    Identifier etaStripId_gap2 = m_rpcIdHelper->channelID(collId,idz,idp,2,0,bestStripEta);

	    if ( msg().level()==MSG::DEBUG ) {
	      msg() << MSG::DEBUG << "In the eta acceptance, strips are:" << endreq;
	      m_rpcIdHelper->print(etaStripId_gap1);
	      m_rpcIdHelper->print(etaStripId_gap2);
	    }

	    closestId[0][0] = etaStripId_gap1;
	    closestId[1][0] = etaStripId_gap2;
	    
	    // find the best phi strips in the two gas gaps
	    // loop on the phi strips
	    for ( int istripphi = 1 ; istripphi<nphi+1 ; ++istripphi) {
	      double stripPhiPos = (readoutEl->stripPos(doubletR,idz,idp,1,1,istripphi)).phi();
	      if ( stripPhiPos<0 ) stripPhiPos += 2.*CLHEP::pi;
	      if ( fabs(phiIntersection-stripPhiPos)<bestPhiDist ) {
		bestPhiDist = fabs(phiIntersection-stripPhiPos);
		bestStripPhi = istripphi;
	      } 
	      
	    } // loop on the phi strips

	    Identifier phiStripId_gap1 = m_rpcIdHelper->channelID(collId,idz,idp,1,1,bestStripPhi);
	    Identifier phiStripId_gap2 = m_rpcIdHelper->channelID(collId,idz,idp,2,1,bestStripPhi);
	    
	    if ( msg().level()==MSG::DEBUG ) {
	      msg() << MSG::DEBUG << "In the phi acceptance, strips are:" << endreq;
	      m_rpcIdHelper->print(phiStripId_gap1);
	      m_rpcIdHelper->print(phiStripId_gap2);
	    }

	    closestId[0][1] = phiStripId_gap1;
	    closestId[1][1] = phiStripId_gap2;

	    // now look for the corresponding matching hits
	    for ( std::vector<IdentifierHash>::const_iterator hash_it_prd = rpcHashList.begin();
		  hash_it_prd != rpcHashList.end() ; ++hash_it_prd ) {
	      
	      Muon::RpcPrepDataContainer::const_iterator rpcColl = rpcContainer->indexFind(*hash_it_prd);
	      
	      if ( rpcColl != rpcContainer->end() ) { 
		
		const Muon::RpcPrepDataCollection* coll = (*rpcColl);
		
		// here you should know if the track is in the acceptance	
		// loop on the hits of the collection
		Muon::RpcPrepDataCollection::const_iterator coll_it = coll->begin();
		for ( ; coll_it != coll->end() ; ++coll_it ) {
		  const Muon::RpcPrepData* prd = *coll_it;
		  Identifier idHit = prd->identify();
		  
		  float residual= 99999999.;
		  
		  if ( !m_rpcIdHelper->measuresPhi(idHit) ) { 

		    float thetaInt = intersection.theta();
		    float dR = prd->globalPosition().perp()-intersection.perp();
		    float dZ = dR/fabs(tan(thetaInt));
		    
		    float zCorr = intersection.z()>0. ? intersection.z()+dZ : intersection.z()-dZ;
		    float residualCorr = zCorr-(prd->globalPosition().z());

		    msg() << MSG::DEBUG << "Position of the intersection: " 
			  << " x: "   << intersection.x()
			  << " y: "   << intersection.y()
			  << " z: "   << intersection.z()
			  << " phi: " << intersection.phi()
			  << " R: "   << intersection.perp() 
			  << " eta: " << intersection.eta()
			  << " theta,dR,dZ: " << intersection.theta() << "," << dR << "," << dZ 
			  << " corr: " << zCorr << endreq;
		    msg() << MSG::DEBUG << "Position of the hit: " 
			  << " x: "   << prd->globalPosition().x()
			  << " y: "   << prd->globalPosition().y()
			  << " z: "   << prd->globalPosition().z()
			  << " phi: " << prd->globalPosition().phi()
			  << " R: "   << prd->globalPosition().perp() 
			  << " residual: " << residual
			  << " residualCorr: " << residualCorr << endreq;
		    
		    /// use as residuals the corrected ones
		    residual= intersection.z()-(prd->globalPosition().z());
		    //		    residual = residualCorr;

		    int gasGap = m_rpcIdHelper->gasGap(idHit)-1;
		    int measPhi = m_rpcIdHelper->measuresPhi(idHit);
		    
		    if ( fabs(residual) < fabs(residualsArray[gasGap][measPhi]) ) {
		      residualsArray[gasGap][measPhi] = residual;
		      //		      closestHitArray[gasGap][measPhi] = prd->identify();
		      closestHitArray[gasGap][measPhi] = *prd;
		    }
		  }
		  else if ( m_rpcIdHelper->measuresPhi(idHit) ) {
		    
		    float phiInt = intersection.phi();
		    if ( phiInt<0. ) phiInt += 2.*CLHEP::pi;
		    //		    float dR = abs(prd->globalPosition().perp()-intersection.perp());
		    //		    float xInt = intersection.x()+dR*cos(phiInt);
		    //		    float yInt = intersection.y()+dR*sin(phiInt);

		    float xInt = intersection.x();
		    float yInt = intersection.y();

		    int sign=(intersection.phi() < prd->globalPosition().phi()) ? -1 : 1;
		    residual=sign*sqrt(pow(xInt-prd->globalPosition().x(),2) +
				       pow(yInt-prd->globalPosition().y(),2));
		    
		    
		    int gasGap = m_rpcIdHelper->gasGap(idHit)-1;
		    int measPhi = m_rpcIdHelper->measuresPhi(idHit);
		    
		    if ( fabs(residual) < fabs(residualsArray[gasGap][measPhi]) ) {
		      msg() << MSG::DEBUG << "Position of the intersection: " 
			    << " x: "   << intersection.x()
			    << " y: "   << intersection.y()
			    << " z: "   << intersection.z()
			    << " phi: " << intersection.phi()
			    << " R: "   << intersection.perp() << endreq;
		      msg() << MSG::DEBUG << "         Position of the hit: " 
			    << " x: "   << prd->globalPosition().x()
			    << " y: "   << prd->globalPosition().y()
			    << " z: "   << prd->globalPosition().z()
			    << " phi: " << prd->globalPosition().phi()
			    << " gasGap: "   << gasGap
			    << " measPhi: "   << measPhi
			    << " R: "   << prd->globalPosition().perp() 
			    << " residuals: " << residual << endreq;
		      
		      
		      residualsArray[gasGap][measPhi] = residual;
		      //		      closestHitArray[gasGap][measPhi] = prd->identify();
		      closestHitArray[gasGap][measPhi] = *prd;
		    } 
		  }
		  
		  
		  
		} // end of the loop on the hits of the collection
	      }
	    
	      // add the intersections and resituals to the output
	      for ( int gasGap = 0 ; gasGap<2 ; ++gasGap ) {
		for ( int measPhi = 0 ; measPhi<2 ; ++measPhi ) {

		  // include the residuals also if a match is not found
		  //		  intersections.push_back(closestId[gasGap][measPhi]);
		  //		  residuals.push_back(residualsArray[gasGap][measPhi]);
		  //		  positions.push_back(extrEta1);
		  
		  if ( msg().level()==MSG::DEBUG ) {
		    std::cout << "RESIDUALS for gasGap, measPhi " << gasGap+1 << ", " << measPhi 
			      << "residuals: " << residualsArray[gasGap][measPhi] << std::endl;
		  }
		  
		  
		  // dump everything if intersection not found !
		  if ( msg().level()==MSG::VERBOSE && inEtaAcceptance && 
		       inPhiAcceptance && residualsArray[gasGap][measPhi]>99999. ) {
		    
		    std::cout << std::endl;
		    
		    std::cout << "##########################################################" << std::endl;
		    std::cout << "##########################################################" << std::endl;
		    
		    std::cout << "GasGap, measPhi: " << gasGap+1 << ", " << measPhi << std::endl;
		    std::cout << "Identifier of the closest ID: " << std::endl;
		    m_rpcIdHelper->print(closestId[gasGap][measPhi]);
		    
		    std::cout << ">>> DUMP of the collection hits::: " << std::endl;
		    // now dump the full content of the hashId
		    if ( rpcColl != rpcContainer->end() ) { 
		      
		      const Muon::RpcPrepDataCollection* coll = (*rpcColl);
		      
		      // here you should know if the track is in the acceptance	
		      // loop on the hits of the collection
		      Muon::RpcPrepDataCollection::const_iterator coll_it = coll->begin();
		      for ( ; coll_it != coll->end() ; ++coll_it ) {
			const Muon::RpcPrepData* prd = *coll_it;
			Identifier idHit = prd->identify();
			m_rpcIdHelper->print(idHit);
			
			// compute the residuals dumping all the shit
			float residual= 0;
			if ( !m_rpcIdHelper->measuresPhi(idHit) && measPhi==0 ) { 
			  residual= intersection.z()-(prd->globalPosition().z());
			  //			  residual= extrEta1.z()-(prd->globalPosition().z());
			  
			  std::cout << "---> IntersezioneZ, hitZ, residualZ: " << extrEta1.z() << ", "
				    << prd->globalPosition().z() << ", " << residual << std::endl;
			}
			else if ( m_rpcIdHelper->measuresPhi(idHit) && measPhi==1 ) {
			  //			  int sign=(extrEta1.phi() < prd->globalPosition().phi()) ? -1 : 1;
			  //			  residual=sign * sqrt(pow(extrEta1.x()-prd->globalPosition().x(),2) +
			  //					       pow(extrEta1.y()-prd->globalPosition().y(),2));
			  int sign=(intersection.phi() < prd->globalPosition().phi()) ? -1 : 1;
			  residual=sign * sqrt(pow(intersection.x()-prd->globalPosition().x(),2) +
					       pow(intersection.y()-prd->globalPosition().y(),2));
			  
			  std::cout << "---> IntersezionePhi, hitPhi, residualPhi: " << extrEta1.phi() << ", "
				    << prd->globalPosition().phi() << ", " << residual << std::endl;
			}
			
			
		      }
		      
		    } // end of the loop on the hits in the dump
		    
		  }  // end of the non-match check dump
		
		}
	      }
	    }
	    
	          // add the intersections and resituals to the output
	    for ( int gasGap = 0 ; gasGap<2 ; ++gasGap ) {
	      for ( int measPhi = 0 ; measPhi<2 ; ++measPhi ) {
		
		// include the residuals also if a match is not found
		intersections.push_back(closestId[gasGap][measPhi]);
		residuals.push_back(residualsArray[gasGap][measPhi]);
		closestHit.push_back(closestHitArray[gasGap][measPhi]);
		positions.push_back(extrEta1);	  
	      }
	    }
	    
	  } // verify that the track is in the acceptance of the detector element of the loop
      
	  else {
	    msg() << MSG::DEBUG << "OOO This chamber is not crossed by the muon !" << endreq;
	  }


	} // loop on the doubletPhi of the readoutelement
      } // loop on the doubletZ of the readoutelement	

    } // end of the loop on the hash id in the region selector    

  } // check if the list from the region selector is empty

  return 0;
}


// dump the RPC panel surfaces
void L1MuonExtrapolator::dumpSurfaces() {

  ATH_MSG_INFO("Dumping the panel parameters");

  std::ofstream dumpFile;
  dumpFile.open("surfaces.txt");

  int nDoubletR   = 2;
  int nDoubletZ   = 2;
  int nDoubletPhi = 2;
  int nGasGap     = 2;
  //int nMeasPhi    = 2;
                             
  // indeces corresponding to the station types                                                                    
  int nStationTypes = 8;
  //  std::string measuresPhi[2] = {"etaStrips","phiStrips"};
  
  // retrieve the histograms
  for ( int istation = 0 ; istation < nStationTypes  ; istation++ ) {
    // loop on the eta and phi sectors
    for ( int stationEta = -7 ; stationEta<8 ; stationEta++) {
      for ( int stationPhi = 1 ; stationPhi<9 ; stationPhi++) {
	
	for ( int idoubletr = 1 ; idoubletr < nDoubletR+1  ; idoubletr++ ) {
	  for ( int idoubletphi = 1 ; idoubletphi < nDoubletPhi+1  ; idoubletphi++ ) {
	    for ( int idoubletz = 1 ; idoubletz < nDoubletZ+1  ; idoubletz++ ) {
	      for ( int igasgap = 1 ; igasgap < nGasGap+1      ; igasgap++ ) {

		std::string  stationTypes[8] = {"BML","BMS","BOL","BOS","BMF","BOG","BOF","BME"};
		int  stationNamesInt[8];
		
		stationNamesInt[0] = 2;
		stationNamesInt[1] = 3;
		stationNamesInt[2] = 4;
		stationNamesInt[3] = 5;
		stationNamesInt[4] = 8;
		stationNamesInt[5] = 9;
		stationNamesInt[6] = 10;
		stationNamesInt[7] = 53;

		if ( ( stationTypes[istation]=="BOL" || stationTypes[istation]=="BOS" ) && idoubletr==2  ) continue;
		
		int imeasphi=0;

		bool isValid;

		Identifier panelId = m_rpcIdHelper->panelID(stationNamesInt[istation],
							    stationEta,
							    stationPhi,
							    idoubletr,idoubletz,idoubletphi,
							    igasgap,imeasphi,true,&isValid);
				
		// now get the rpc readout element
		const MuonGM::RpcReadoutElement* readoutEl = m_muonMgr->getRpcReadoutElement(panelId);
		if ( !readoutEl ) {
		  ATH_MSG_WARNING("Could not find the RPC readout element");
		  continue;
		} 
		std::cout << "P4-> Now the station name is: " << stationNamesInt[istation] << std::endl;
		
		int neta = readoutEl->NetaStrips();
		int nphi = readoutEl->NphiStrips();
		
		Amg::Vector3D gap1_firstStripEtaPos = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,1,0,1);
		Amg::Vector3D gap1_lastStripEtaPos  = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,1,0,neta);
		Amg::Vector3D gap2_firstStripEtaPos = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,2,0,1);
		Amg::Vector3D gap2_lastStripEtaPos  = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,2,0,neta);
		
		Amg::Vector3D gap1_firstStripPhiPos = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,1,1,1);
		Amg::Vector3D gap1_lastStripPhiPos  = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,1,1,nphi);
		Amg::Vector3D gap2_firstStripPhiPos = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,2,1,1);
		Amg::Vector3D gap2_lastStripPhiPos  = readoutEl->stripPos(idoubletr,idoubletz,idoubletphi,2,1,nphi);
		
		// get the surface
		float etaDist1 = Amg::distance(gap1_firstStripEtaPos,gap1_lastStripEtaPos);
		float phiDist1 = Amg::distance(gap1_firstStripPhiPos,gap1_lastStripPhiPos);
		
		float etaDist2 = Amg::distance(gap2_firstStripEtaPos,gap2_lastStripEtaPos);
		float phiDist2 = Amg::distance(gap2_firstStripPhiPos,gap2_lastStripPhiPos);
		
		float surface1 = etaDist1 * phiDist1;
		float surface2 = etaDist2 * phiDist2;
		
		if ( igasgap==1 ) { 
		  dumpFile << stationNamesInt[istation] << " " << stationEta << " " << stationPhi 
			   << " " << idoubletr << " " << idoubletphi << " " << idoubletz << " " << igasgap << " " 
			   << surface1 << " " 
			   << gap1_firstStripEtaPos.x() << " " 
			   << gap1_firstStripEtaPos.y() << " " 
			   << gap1_firstStripEtaPos.z() << " "
			   << gap1_lastStripEtaPos.x() << " " 
			   << gap1_lastStripEtaPos.y() << " " 
			   << gap1_lastStripEtaPos.z() << " "
			   << gap1_firstStripPhiPos.x() << " " 
			   << gap1_firstStripPhiPos.y() << " " 
			   << gap1_firstStripPhiPos.z() << " "
			   << gap1_lastStripPhiPos.x() << " " 
			   << gap1_lastStripPhiPos.y() << " " 
			   << gap1_lastStripPhiPos.z() << " "
			   << std::endl;
		}
		  else if ( igasgap==2 ) {
		    dumpFile << stationNamesInt[istation] << " " << stationEta << " " << stationPhi 
			     << " " << idoubletr << " " << idoubletphi << " " << idoubletz << " " << igasgap << " " 
			     << surface2 << " " 
			     << gap2_firstStripEtaPos.x() << " " 
			     << gap2_firstStripEtaPos.y() << " " 
			     << gap2_firstStripEtaPos.z() << " "
			     << gap2_lastStripEtaPos.x() << " " 
			     << gap2_lastStripEtaPos.y() << " " 
			     << gap2_lastStripEtaPos.z() << " "
			     << gap2_firstStripPhiPos.x() << " " 
			     << gap2_firstStripPhiPos.y() << " " 
			     << gap2_firstStripPhiPos.z() << " "
			     << gap2_lastStripPhiPos.x() << " " 
			     << gap2_lastStripPhiPos.y() << " " 
			     << gap2_lastStripPhiPos.z() << " "
			     << std::endl;
		  }

		  
		  //		}  // check if it's valid 
		
		
		
	      }
	    }
	    
	  }
	}

      }
    }
  }          

  dumpFile.close();

  return;
}
