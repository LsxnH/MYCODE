
// C/C++
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>

// Local
#include "VarDefs.h"

#define REGISTER_ANP_VAR(VAR) RegisterVar( VAR, #VAR)

using namespace std;

//======================================================================================================
namespace Ath
{
  namespace Def
  {
    typedef std::map<std::string, uint32_t> VarMap;
    
    static VarMap gAthVars;

    static int VarFlag = RegisterAllEnums();

    static bool gDebugAthVars = false;
  }
}

//======================================================================================================
bool Ath::Def::RegisterVar(uint32_t var, const std::string &name)
{
  if(Def::IsKnownKey(name)) {
    cout << "Def::RegisterVar - WARNING - variable with this name already exists: var=" << var << ", name=" << name << endl;
    return false;      
  }

  pair<VarMap::iterator, bool> vit = gAthVars.insert(VarMap::value_type(name, var));

  if(!vit.second) {
    cout << "Def::RegisterVar - WARNING - variable with this key already exists: var=" << var << ", name=" << name << endl;
    return false;
  }

  if(gDebugAthVars) {
    cout << "Def::RegisterVar - DEBUG - add variable: " << var << ", name=\"" << name << "\"" << endl;
  }

  return true;
}

//======================================================================================================
uint32_t Ath::Def::RegisterDynamicVar(const std::string &name)
{
  //
  // Register new variable that is not on the enum list
  //
  const VarMap::const_iterator fit = gAthVars.find(name);

  if(fit != gAthVars.end()) {
    return fit->second;
  }

  const uint32_t var = FindHighestKey()+1;

  if(!RegisterVar(var, name)) {
    cout << "RegisterDynamicVar - failed to register the variable: " << endl
	 << "   var:  " << var  << endl
	 << "   name: " << name << endl;
  }

  return var;
}

//======================================================================================================
std::string Ath::Def::AsStr(uint32_t var)
{
  return Convert2Str(var);
}

//======================================================================================================
std::vector<std::string> Ath::Def::GetAllVarNames()
{
  vector<string> names;

  for(const VarMap::value_type &v: gAthVars) {
    names.push_back(v.first);
  }

  return names;
}

//======================================================================================================
std::vector<uint32_t> Ath::Def::GetAllVarKeys()
{
  vector<uint32_t> enums;

  for(const VarMap::value_type &v: gAthVars) {
    enums.push_back(v.second);
  }

  return enums;
}

//======================================================================================================
uint32_t Ath::Def::Convert2Var(const std::string &var)
{
  //
  // Find key for matching variable
  //
  const VarMap::const_iterator fit = gAthVars.find(var);

  if(fit != gAthVars.end()) {
    return fit->second;
  }

  if(gDebugAthVars) {
    cout << "Def::Convert2Var - WARNING - unknown variable key: \"" << var << "\"" << endl;
  }

  return NONE;
}

//======================================================================================================
std::string Ath::Def::Convert2Str(uint32_t var)
{
  for(const VarMap::value_type &v: gAthVars) {
    if(v.second == var) {
      return v.first;
    }
  }

  if(gDebugAthVars) {
    cout << "Def::Convert2Str - WARNING - unknown variable: " << var << endl;
  }

  static string snone = "NONE";
  return snone;
}

//======================================================================================================
void Ath::Def::PrintVarNames()
{
  vector<uint32_t> vars = Def::GetAllVarKeys();
  std::sort(vars.begin(), vars.end());
  
  cout << "PrintVarNames - print " << vars.size() << " enum(s)" << endl;
  
  for(uint32_t i = 0; i < vars.size(); ++i) {
    cout << "   " << setw(10) << vars.at(i) << ": \"" << Def::AsStr(vars.at(i)) << "\"" << endl;
  }
}

//======================================================================================================
std::string Ath::Def::AsStr(uint32_t key, float val)
{
  stringstream s;

  if(key != NONE) {
    s << AsStr(key) << ": " << val;
  }
  else {
    s <<       key  << ": " << val;
  }

  return s.str();
}

//======================================================================================================
uint32_t Ath::Def::FindHighestKey()
{
  uint32_t hkey = NONE;

  for(const VarMap::value_type &v: gAthVars) {
    hkey = std::max<uint32_t>(hkey, v.second);
  }

  return hkey;
}

//======================================================================================================
bool Ath::Def::IsKnownKey(const string &key)
{
  return gAthVars.find(key) != gAthVars.end();
}

//======================================================================================================
bool Ath::Def::IsKnownVar(uint32_t key)
{
  for(const VarMap::value_type &v: gAthVars) {
    if(key == v.second) {
      return true;
    }
  }

  return false;
}

//======================================================================================================
int Ath::Def::RegisterAllEnums()
{
  REGISTER_ANP_VAR( NONE );
  REGISTER_ANP_VAR( Et );
  REGISTER_ANP_VAR( Energy );
  REGISTER_ANP_VAR( Eta );
  REGISTER_ANP_VAR( Phi );
  REGISTER_ANP_VAR( Pt );
  REGISTER_ANP_VAR( PtOrg );
  REGISTER_ANP_VAR( Px );
  REGISTER_ANP_VAR( Py );
  REGISTER_ANP_VAR( Pz );
  REGISTER_ANP_VAR( Mass );
  REGISTER_ANP_VAR( PDG );
  REGISTER_ANP_VAR( Charge );
  REGISTER_ANP_VAR( VtxX );
  REGISTER_ANP_VAR( VtxY );
  REGISTER_ANP_VAR( VtxZ );
  REGISTER_ANP_VAR( Author );
  REGISTER_ANP_VAR( Barcode );
  REGISTER_ANP_VAR( TrueBarcode );
  REGISTER_ANP_VAR( VtxBarcode );
  REGISTER_ANP_VAR( Status );
  REGISTER_ANP_VAR( ProdVtxBarcode );
  REGISTER_ANP_VAR( DecayVtxBarcode );

  REGISTER_ANP_VAR( Event );
  REGISTER_ANP_VAR( LumiBlock );
  REGISTER_ANP_VAR( Run );
  REGISTER_ANP_VAR( Year );
  REGISTER_ANP_VAR( bcid );
  REGISTER_ANP_VAR( PassEventCleaning );
  REGISTER_ANP_VAR( backgroundFlags );
  REGISTER_ANP_VAR( MCChannel );
  REGISTER_ANP_VAR( MCWeight );
  REGISTER_ANP_VAR( SimulationType );
  REGISTER_ANP_VAR( EventFlag_Core );
  REGISTER_ANP_VAR( ErrorState_Core );
  REGISTER_ANP_VAR( ErrorState_Background );
  REGISTER_ANP_VAR( ErrorState_LAr );
  REGISTER_ANP_VAR( ErrorState_Tile );
  REGISTER_ANP_VAR( ErrorState_SCT );
  REGISTER_ANP_VAR( ErrorState_Pixel );
  REGISTER_ANP_VAR( ErrorState_TRT );
  REGISTER_ANP_VAR( ErrorState_Muon );

  REGISTER_ANP_VAR( BeamPosX );
  REGISTER_ANP_VAR( BeamPosY );
  REGISTER_ANP_VAR( BeamPosZ );
  REGISTER_ANP_VAR( BeamPosSigmaX );
  REGISTER_ANP_VAR( BeamPosSigmaY );
  REGISTER_ANP_VAR( BeamPosSigmaZ );

  REGISTER_ANP_VAR( ORHFJets_PassSelection );
  REGISTER_ANP_VAR( ORHFJets_PassBJet );
  REGISTER_ANP_VAR( ORHFJets_IsRemoved );

  REGISTER_ANP_VAR( MV2c10_60_SF );
  REGISTER_ANP_VAR( MV2c10_70_SF );
  REGISTER_ANP_VAR( MV2c10_77_SF );
  REGISTER_ANP_VAR( MV2c10_85_SF );

  REGISTER_ANP_VAR( MV2c10_60_Eff );
  REGISTER_ANP_VAR( MV2c10_70_Eff );
  REGISTER_ANP_VAR( MV2c10_77_Eff );
  REGISTER_ANP_VAR( MV2c10_85_Eff );

  REGISTER_ANP_VAR( MV2c10_60_Pass );
  REGISTER_ANP_VAR( MV2c10_70_Pass );
  REGISTER_ANP_VAR( MV2c10_77_Pass );
  REGISTER_ANP_VAR( MV2c10_85_Pass );

  REGISTER_ANP_VAR( PileupWeight );
  REGISTER_ANP_VAR( CorrectedMu );
  REGISTER_ANP_VAR( ActualInteractions );
  REGISTER_ANP_VAR( AverageInteractions );
  REGISTER_ANP_VAR( RandomRunNumber );
  REGISTER_ANP_VAR( RandomLumiBlockNumber );
  REGISTER_ANP_VAR( RandomYear );


  REGISTER_ANP_VAR( LepCorrSF_Reconstruction );
  REGISTER_ANP_VAR( LepCorrSF_Loose );
  REGISTER_ANP_VAR( LepCorrSF_LooseBLayer );
  REGISTER_ANP_VAR( LepCorrSF_Medium );
  REGISTER_ANP_VAR( LepCorrSF_Tight );
  REGISTER_ANP_VAR( LepCorrSF_HighPt );
  REGISTER_ANP_VAR( LepCorrSF_TTVA );

  REGISTER_ANP_VAR( LepCorrSF_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_isoFixedCutTightTrackOnly );
  REGISTER_ANP_VAR( LepCorrSF_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_isoLoose );

  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Loose );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Medium );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Tight );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_HighPt );

  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Loose );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Medium );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Tight );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_HighPt );


  REGISTER_ANP_VAR( LepCorrSF_LooseBLayer_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_LooseBLayer_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_LooseBLayer_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_LooseBLayer_isoLoose );

  REGISTER_ANP_VAR( LepCorrSF_Medium_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_Medium_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_Medium_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_Medium_isoLoose );

  REGISTER_ANP_VAR( LepCorrSF_Tight_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_Tight_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_Tight_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_Tight_isoLoose );


  REGISTER_ANP_VAR( LepCorrSF_Trig1L_LooseBLayer_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_LooseBLayer_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_LooseBLayer_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_LooseBLayer_isoLoose );

  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Medium_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Medium_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Medium_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Medium_isoLoose );

  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Tight_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Tight_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Tight_isoGradient );
  REGISTER_ANP_VAR( LepCorrSF_Trig1L_Tight_isoLoose );


  REGISTER_ANP_VAR( LepCorrEff_Trig1L_LooseBLayer_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_LooseBLayer_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_LooseBLayer_isoGradient );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_LooseBLayer_isoLoose );

  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Medium_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Medium_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Medium_isoGradient );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Medium_isoLoose );

  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Tight_isoFixedCutLoose );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Tight_isoFixedCutTight );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Tight_isoGradient );
  REGISTER_ANP_VAR( LepCorrEff_Trig1L_Tight_isoLoose );


  REGISTER_ANP_VAR( PUWeight );
  REGISTER_ANP_VAR( JVTWeight );
  REGISTER_ANP_VAR( lepSFObjLoose );
  REGISTER_ANP_VAR( lepSFObjTight );
  REGISTER_ANP_VAR( lepSFTrigLoose );
  REGISTER_ANP_VAR( lepSFTrigTight );
  REGISTER_ANP_VAR( MV2c20_77_EventWeight );
  REGISTER_ANP_VAR( MET_RefFinal_et );
  REGISTER_ANP_VAR( MET_RefFinal_phi );

  REGISTER_ANP_VAR( NChildren );
  REGISTER_ANP_VAR( NParents );
  REGISTER_ANP_VAR( IndexRangeChild );
  REGISTER_ANP_VAR( IndexRangeParent );
  REGISTER_ANP_VAR( IndexRangeSignalParent );
    
  REGISTER_ANP_VAR( D0 );
  REGISTER_ANP_VAR( D0Sig );
  REGISTER_ANP_VAR( Z0 );
  REGISTER_ANP_VAR( Z0Sin );
  REGISTER_ANP_VAR( Z0Sig );
  REGISTER_ANP_VAR( nPixHits );
  REGISTER_ANP_VAR( nPixHoles );
  REGISTER_ANP_VAR( nPixelOutliers );
  REGISTER_ANP_VAR( nSCTHits );
  REGISTER_ANP_VAR( nSCTHoles );
  REGISTER_ANP_VAR( nSCTOutliers );
  REGISTER_ANP_VAR( nTRTHits );
  REGISTER_ANP_VAR( nTRTHoles );
  REGISTER_ANP_VAR( nTRTOutliers );
  REGISTER_ANP_VAR( nBLHits );
  REGISTER_ANP_VAR( nBLayerOutliers );
  REGISTER_ANP_VAR( expectBLayerHit );
  REGISTER_ANP_VAR( nIBLHits );
  REGISTER_ANP_VAR( nIBLayerOutliers );
  REGISTER_ANP_VAR( expectIBLayerHit );
  REGISTER_ANP_VAR( chi2 );
  REGISTER_ANP_VAR( ndof );
  REGISTER_ANP_VAR( qoverp );
  REGISTER_ANP_VAR( vx );
  REGISTER_ANP_VAR( vy );
  REGISTER_ANP_VAR( vz );

  REGISTER_ANP_VAR( covariance );
  REGISTER_ANP_VAR( type );

  REGISTER_ANP_VAR( PtCone20 );
  REGISTER_ANP_VAR( PtCone30 );
  REGISTER_ANP_VAR( PtCone40 );
  REGISTER_ANP_VAR( EtCone20 );
  REGISTER_ANP_VAR( EtCone30 );
  REGISTER_ANP_VAR( EtCone40 );

  REGISTER_ANP_VAR( PtVarCone20 );
  REGISTER_ANP_VAR( PtVarCone30 );
  REGISTER_ANP_VAR( PtVarCone40 );

  REGISTER_ANP_VAR( TopoEtCone20 );
  REGISTER_ANP_VAR( TopoEtCone30 );
  REGISTER_ANP_VAR( TopoEtCone40 );

  REGISTER_ANP_VAR( EtaBE2 );
  REGISTER_ANP_VAR( ClEta );
  REGISTER_ANP_VAR( ClPhi );
  REGISTER_ANP_VAR( ClEt );

  REGISTER_ANP_VAR( isLooseBad );
  REGISTER_ANP_VAR( isTightBad );

  REGISTER_ANP_VAR( isLooseLH );
  REGISTER_ANP_VAR( isLooseLHWithBL );
  REGISTER_ANP_VAR( isMediumLH );
  REGISTER_ANP_VAR( isTightLH );

  REGISTER_ANP_VAR( LHLoose );
  REGISTER_ANP_VAR( LHMedium );
  REGISTER_ANP_VAR( LHTight );
  REGISTER_ANP_VAR( LHValue );
  
  REGISTER_ANP_VAR( DFCommonElectronsLHLoose );
  REGISTER_ANP_VAR( DFCommonElectronsLHMedium );
  REGISTER_ANP_VAR( DFCommonElectronsLHTight );

  REGISTER_ANP_VAR( emfrac );
  REGISTER_ANP_VAR( JVT );
  REGISTER_ANP_VAR( Jvt );
  REGISTER_ANP_VAR( SV1 );
  REGISTER_ANP_VAR( IP2D );
  REGISTER_ANP_VAR( IP3D );
  REGISTER_ANP_VAR( JetF );
  REGISTER_ANP_VAR( JetFC );
  REGISTER_ANP_VAR( DL1 );
  REGISTER_ANP_VAR( DL1mu );
  REGISTER_ANP_VAR( DL1rnn );
  REGISTER_ANP_VAR( rnnip );
  REGISTER_ANP_VAR( MV2c00 );
  REGISTER_ANP_VAR( MV2c10 );
  REGISTER_ANP_VAR( MV2c20 );
  REGISTER_ANP_VAR( MV2c100 );
  REGISTER_ANP_VAR( MV2cl100 );
  REGISTER_ANP_VAR( MV2c10mu );
  REGISTER_ANP_VAR( MV2c10rnn );
  REGISTER_ANP_VAR( TileStatus );

  REGISTER_ANP_VAR( JVTPass );
  REGISTER_ANP_VAR( JVTPassDefault );
  REGISTER_ANP_VAR( JVTPassLoose );

  REGISTER_ANP_VAR( JVTSF_Eff );
  REGISTER_ANP_VAR( JVTSF_Ineff );
  REGISTER_ANP_VAR( JVTSF_LooseEff );
  REGISTER_ANP_VAR( JVTSF_LooseIneff );

  REGISTER_ANP_VAR( JetBDTSigVeryLoose );
  REGISTER_ANP_VAR( JetBDTSigLoose );
  REGISTER_ANP_VAR( JetBDTSigMedium );
  REGISTER_ANP_VAR( JetBDTSigTight );
  REGISTER_ANP_VAR( JetBDTScore );

  REGISTER_ANP_VAR( EleBDTLoose );
  REGISTER_ANP_VAR( EleBDTMedium );
  REGISTER_ANP_VAR( EleBDTTight );
  REGISTER_ANP_VAR( EleBDTScore );

  REGISTER_ANP_VAR( PassEleOLR );
  REGISTER_ANP_VAR( MuonVeto   );
  REGISTER_ANP_VAR( DecayMode  );

  REGISTER_ANP_VAR( SumPtTrkPt500 );
  REGISTER_ANP_VAR( NumTrkPt500 );
  REGISTER_ANP_VAR( EMFrac );
  REGISTER_ANP_VAR( HECQuality );
  REGISTER_ANP_VAR( LArQuality );
  REGISTER_ANP_VAR( DetectorEta );
  REGISTER_ANP_VAR( PartonTruthLabelID );
  REGISTER_ANP_VAR( ConeTruthLabelID );
  REGISTER_ANP_VAR( HadronConeExclTruthLabelID );

  REGISTER_ANP_VAR( NRecoVtx );
  REGISTER_ANP_VAR( HasPriVtx );
  REGISTER_ANP_VAR( PriVtxX );
  REGISTER_ANP_VAR( PriVtxY );
  REGISTER_ANP_VAR( PriVtxZ );

  REGISTER_ANP_VAR( NTrack );
  REGISTER_ANP_VAR( TrackPt );
  REGISTER_ANP_VAR( TrackEta );
  REGISTER_ANP_VAR( TrackTheta );
  REGISTER_ANP_VAR( TrackPhi );
  REGISTER_ANP_VAR( TrackD0 );
  REGISTER_ANP_VAR( TrackD02D );
  REGISTER_ANP_VAR( TrackD0Sig );
  REGISTER_ANP_VAR( TrackZ0Sin );
  REGISTER_ANP_VAR( TrackZ0Sig );
  REGISTER_ANP_VAR( TrackNPIX );
  REGISTER_ANP_VAR( TrackNPIXHole );
  REGISTER_ANP_VAR( TrackNSCT );
  REGISTER_ANP_VAR( TrackNSCTHole );
  REGISTER_ANP_VAR( TrackNIBL );
  REGISTER_ANP_VAR( TrackNBL );
  REGISTER_ANP_VAR( TrackExpectIBL );
  REGISTER_ANP_VAR( TrackExpectBL );
  REGISTER_ANP_VAR( TrackChi2 );
  REGISTER_ANP_VAR( TrackNdof );
  REGISTER_ANP_VAR( TrackBarcode );

  REGISTER_ANP_VAR( AllTrackPt );
  REGISTER_ANP_VAR( AllTrackEta );
  REGISTER_ANP_VAR( AllTrackPhi );
  REGISTER_ANP_VAR( AllTrackD0 );
  REGISTER_ANP_VAR( AllTrackD02D );
  REGISTER_ANP_VAR( AllTrackD0Sig );
  REGISTER_ANP_VAR( AllTrackZ0Sin );
  REGISTER_ANP_VAR( AllTrackZ0Sig );
  REGISTER_ANP_VAR( AllTrackNPIX );
  REGISTER_ANP_VAR( AllTrackNPIXHole );
  REGISTER_ANP_VAR( AllTrackNSCT );
  REGISTER_ANP_VAR( AllTrackNSCTHole );
  REGISTER_ANP_VAR( AllTrackNIBL );
  REGISTER_ANP_VAR( AllTrackNBL );
  REGISTER_ANP_VAR( AllTrackExpectIBL );
  REGISTER_ANP_VAR( AllTrackExpectBL );
  REGISTER_ANP_VAR( AllTrackChi2 );
  REGISTER_ANP_VAR( AllTrackNdof );
  REGISTER_ANP_VAR( AllTrackBarcode );

  REGISTER_ANP_VAR( VeryLoose );
  REGISTER_ANP_VAR( Loose );
  REGISTER_ANP_VAR( Medium );
  REGISTER_ANP_VAR( Tight );
  REGISTER_ANP_VAR( HighPt );
  REGISTER_ANP_VAR( Quality );
  REGISTER_ANP_VAR( IsGoodQuality );
  REGISTER_ANP_VAR( PassedID );

  REGISTER_ANP_VAR( isoTight );
  REGISTER_ANP_VAR( isoLooseTrackOnly );
  REGISTER_ANP_VAR( isoLoose );
  REGISTER_ANP_VAR( isoGradient );
  REGISTER_ANP_VAR( isoGradientLoose );
  REGISTER_ANP_VAR( isoFixedCutTight );
  REGISTER_ANP_VAR( isoFixedCutTightTrackOnly );
  REGISTER_ANP_VAR( isoFixedCutLoose );

  REGISTER_ANP_VAR( firstEgMotherPdgId );
  REGISTER_ANP_VAR( firstEgMotherTruthOrigin );
  REGISTER_ANP_VAR( firstEgMotherTruthType );
  REGISTER_ANP_VAR( truthOrigin );
  REGISTER_ANP_VAR( truthPdgId );
  REGISTER_ANP_VAR( truthType );
  REGISTER_ANP_VAR( bkgMotherPdgId );
  REGISTER_ANP_VAR( bkgTruthOrigin );
  REGISTER_ANP_VAR( bkgTruthType );

  REGISTER_ANP_VAR( TruthJetDR );
  REGISTER_ANP_VAR( TruthJetPt );
  REGISTER_ANP_VAR( TruthJetID );

  REGISTER_ANP_VAR( JetEMScaleMomentum_phi );
  REGISTER_ANP_VAR( JetEMScaleMomentum_eta );
  REGISTER_ANP_VAR( JetEMScaleMomentum_pt );
  REGISTER_ANP_VAR( JetEMScaleMomentum_m );

  //
  // Btagging MVA vars
  //
  REGISTER_ANP_VAR( pt_uCalib );
  REGISTER_ANP_VAR( eta_uCalib );
  REGISTER_ANP_VAR( eta_abs_uCalib );
  REGISTER_ANP_VAR( truthLabel );
  REGISTER_ANP_VAR( ip2_c );
  REGISTER_ANP_VAR( ip2_cu );
  REGISTER_ANP_VAR( ip3_c );
  REGISTER_ANP_VAR( ip3_cu );
  REGISTER_ANP_VAR( ip2 );
  REGISTER_ANP_VAR( ip3 );
  REGISTER_ANP_VAR( sv1_mass );
  REGISTER_ANP_VAR( sv1_efrc );
  REGISTER_ANP_VAR( sv1_Lxy );
  REGISTER_ANP_VAR( sv1_L3d );
  REGISTER_ANP_VAR( sv1_sig3 );
  REGISTER_ANP_VAR( sv1_dR );
  REGISTER_ANP_VAR( sv1_ntkv );
  REGISTER_ANP_VAR( sv1_n2t );
  REGISTER_ANP_VAR( jf_mass );
  REGISTER_ANP_VAR( jf_efrc );
  REGISTER_ANP_VAR( jf_dR );
  REGISTER_ANP_VAR( jf_sig3 );
  REGISTER_ANP_VAR( jf_n2tv );
  REGISTER_ANP_VAR( jf_ntrkv );
  REGISTER_ANP_VAR( jf_nvtx );
  REGISTER_ANP_VAR( jf_nvtx1t );

  REGISTER_ANP_VAR( MET_PassSelection );
  REGISTER_ANP_VAR( MET_LCTopo );
  REGISTER_ANP_VAR( MET_LCTopo_Phi );
  REGISTER_ANP_VAR( MET_LCTopo_SumEt );
  REGISTER_ANP_VAR( MET_Track );
  REGISTER_ANP_VAR( MET_Track_Phi );
  REGISTER_ANP_VAR( MET_Track_SumEt );

  return 1;
}

