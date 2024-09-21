#ifndef TRIGMUONANALYSIS_L1MUONEXTRAPOLATOR_H
#define TRIGMUONANALYSIS_L1MUONEXTRAPOLATOR_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : L1MuonExtrapolator
 * @Author : Stefano Rosati
 *
 * @Brief  : This class is copied from MuonTriggerValidation
 * 
 **********************************************************************************/

#include "AthenaBaseComps/AthAlgTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "xAODMuon/MuonContainer.h"
//#include "RegionSelector/IRegSelSvc.h"
#include "MuonPrepRawData/RpcPrepDataContainer.h"


class RpcIdHelper;
namespace Muon {
  class MuonIdHelperTool;
  class MuonTGMeasurementTool;
}

namespace MuonGM {
  class MuonDetectorManager;
  class RpcDetectorElement;
}

namespace Trk { 
  class IExtrapolator;
}

class L1MuonExtrapolator : public AthAlgTool
{

 public:

  static const InterfaceID& interfaceID();

 public:
  
  L1MuonExtrapolator(const std::string& type,
		     const std::string& name,
		     const IInterface* parent);
  
  ~L1MuonExtrapolator();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();


  // get the intersections of the track with the RPC stations
  int getIntersections(const xAOD::Muon* muon, std::vector<Identifier>& intersections, 
		       std::vector<Amg::Vector3D>& positions, std::vector<float>& residuals,
		       std::vector<Muon::RpcPrepData>& closestHit);
  
  // dump the surfaces and borders of each panel
  void dumpSurfaces();

 private:
  
  //  ToolHandle<RpcExtrapolationTool> m_extrapolationTool;
  //  std::vector<RpcExtrapolationResults> m_intersections;
  const MuonGM::MuonDetectorManager* m_muonMgr;                   
        
  ToolHandle<Muon::MuonIdHelperTool> m_idHelperTool;
  ToolHandle<Trk::IExtrapolator>      m_extrapolator;

  const Trk::TrackStateOnSurface* getClosestSurface(const xAOD::TrackParticle* tp, 
						    const MuonGM::RpcDetectorElement* detEl);

  float getPhiCorrection(float phiIntersection, float centralPhiStrip, float centralPhiR);
  
  //  IRegSelSvc* m_regionSelector;

  const RpcIdHelper* m_rpcIdHelper;
  //  std::vector<int> m_matchingIntersections;

};

#endif
