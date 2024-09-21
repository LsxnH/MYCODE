#ifndef TRIGMUONANALYSIS_L1MUONHISTOTOOL_H
#define TRIGMUONANALYSIS_L1MUONHISTOTOOL_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : L1MuonHistoTool
 * @Author : Stefano Rosati
 *
 * @Brief  : This class is copied from MuonTriggerValidation
 * 
 **********************************************************************************/

#include "AthenaBaseComps/AthHistogramTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "TH2F.h"

class RpcIdHelper;
class Identifier;

class L1MuonHistoTool : public AthHistogramTool
{

 public:

  static const InterfaceID& interfaceID();

  L1MuonHistoTool(const std::string& type,
		  const std::string& name,
		  const IInterface* parent);
  
  ~L1MuonHistoTool();
  
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  StatusCode bookHistos();
  
  void fillHistos(std::vector<Identifier>& intersections, std::vector<float>& residuals);

  void writeHistos();

 private:

  // efficiency histograms (numerator and denominator) 
  TH2F* m_numEtaPhi[8][2][2][2];
  TH2F* m_denEtaPhi[8][2][2][2];

  TH2F* m_numEtaPhi_hw[8][2][2][2];
  TH2F* m_denEtaPhi_hw[8][2][2][2];

  const RpcIdHelper* m_rpcIdHelper;
  
  
};

#endif
