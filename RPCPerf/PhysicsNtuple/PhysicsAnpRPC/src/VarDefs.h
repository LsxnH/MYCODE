// This is -*- c++ -*-
#ifndef ATH_VARDEFS_H
#define ATH_VARDEFS_H

/**********************************************************************************
 * @Package: PhysicsLight
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  List of event variables
 * 
 **********************************************************************************/

// C/C++
#include <string>
#include <vector>
#include <stdint.h>

// ROOT
#include "Rtypes.h"

namespace Ath
{
  namespace Def
  {
    bool RegisterVar(unsigned var, const std::string &name);

    uint32_t RegisterDynamicVar(const std::string &name);

    std::vector<std::string> GetAllVarNames();
    std::vector<unsigned>    GetAllVarKeys ();

    std::string       AsStr(uint32_t key, float val);
    std::string       AsStr(unsigned var);
    std::string Convert2Str(unsigned var);
    uint32_t    Convert2Var(const std::string &var);   

    uint32_t    FindHighestKey();

    bool        IsKnownKey(const std::string &key);
    bool        IsKnownVar(uint32_t key);

    void PrintVarNames();

    int RegisterAllEnums();

    //
    // Define explicit variables
    //
    enum Var 
    {
      NONE = 5100000,
      Et,      
      Energy,
      Eta,
      Phi,
      Pt,
      PtOrg,
      Px,
      Py,
      Pz,
      Mass,
      PDG,
      Charge, 
      VtxX,
      VtxY,
      VtxZ,
      Author,
      Barcode,
      TrueBarcode,
      VtxBarcode,
      Status,
      ProdVtxBarcode,
      DecayVtxBarcode,
      NChildren,
      NParents,
      IndexRangeChild,
      IndexRangeParent,
      IndexRangeSignalParent,

      Event,
      LumiBlock,
      Run,
      Year,
      bcid,
      backgroundFlags,
      PassEventCleaning,
      MCChannel,
      MCWeight,
      SimulationType,
      EventFlag_Core,
      ErrorState_Core,
      ErrorState_Background,
      ErrorState_LAr,
      ErrorState_Tile,
      ErrorState_SCT,
      ErrorState_Pixel,
      ErrorState_TRT,
      ErrorState_Muon,

      BeamPosX,
      BeamPosY,
      BeamPosZ,
      BeamPosSigmaX,
      BeamPosSigmaY,
      BeamPosSigmaZ,

      ORHFJets_PassSelection,
      ORHFJets_PassBJet,
      ORHFJets_IsRemoved,

      MV2c10_60_SF,
      MV2c10_70_SF,
      MV2c10_77_SF,
      MV2c10_85_SF,

      MV2c10_60_Eff,
      MV2c10_70_Eff,
      MV2c10_77_Eff,
      MV2c10_85_Eff,

      MV2c10_60_Pass,
      MV2c10_70_Pass,
      MV2c10_77_Pass,
      MV2c10_85_Pass,

      PileupWeight,
      CorrectedMu,
      ActualInteractions,
      AverageInteractions,
      RandomRunNumber,
      RandomLumiBlockNumber,
      RandomYear,

      LepCorrSF_Reconstruction,
      LepCorrSF_Loose,
      LepCorrSF_LooseBLayer,
      LepCorrSF_Medium,
      LepCorrSF_Tight,
      LepCorrSF_HighPt,
      LepCorrSF_TTVA,

      LepCorrSF_isoFixedCutLoose,
      LepCorrSF_isoFixedCutTight,
      LepCorrSF_isoFixedCutTightTrackOnly,
      LepCorrSF_isoGradient,
      LepCorrSF_isoLoose,
   

      LepCorrSF_LooseBLayer_isoFixedCutLoose,
      LepCorrSF_LooseBLayer_isoFixedCutTight,
      LepCorrSF_LooseBLayer_isoGradient,
      LepCorrSF_LooseBLayer_isoLoose,

      LepCorrSF_Medium_isoFixedCutLoose,
      LepCorrSF_Medium_isoFixedCutTight,
      LepCorrSF_Medium_isoGradient,
      LepCorrSF_Medium_isoLoose,

      LepCorrSF_Tight_isoFixedCutLoose,
      LepCorrSF_Tight_isoFixedCutTight,
      LepCorrSF_Tight_isoGradient,
      LepCorrSF_Tight_isoLoose,
      
      LepCorrSF_Trig1L_Loose,
      LepCorrSF_Trig1L_Medium,
      LepCorrSF_Trig1L_Tight,
      LepCorrSF_Trig1L_HighPt,

      LepCorrEff_Trig1L_Loose,
      LepCorrEff_Trig1L_Medium,
      LepCorrEff_Trig1L_Tight,
      LepCorrEff_Trig1L_HighPt,
      
      
      LepCorrSF_Trig1L_LooseBLayer_isoFixedCutLoose,
      LepCorrSF_Trig1L_LooseBLayer_isoFixedCutTight,
      LepCorrSF_Trig1L_LooseBLayer_isoGradient,
      LepCorrSF_Trig1L_LooseBLayer_isoLoose,
      
      LepCorrSF_Trig1L_Medium_isoFixedCutLoose,
      LepCorrSF_Trig1L_Medium_isoFixedCutTight,
      LepCorrSF_Trig1L_Medium_isoGradient,
      LepCorrSF_Trig1L_Medium_isoLoose,
      
      LepCorrSF_Trig1L_Tight_isoFixedCutLoose,
      LepCorrSF_Trig1L_Tight_isoFixedCutTight,
      LepCorrSF_Trig1L_Tight_isoGradient,
      LepCorrSF_Trig1L_Tight_isoLoose,
      
      LepCorrEff_Trig1L_LooseBLayer_isoFixedCutLoose,
      LepCorrEff_Trig1L_LooseBLayer_isoFixedCutTight,
      LepCorrEff_Trig1L_LooseBLayer_isoGradient,
      LepCorrEff_Trig1L_LooseBLayer_isoLoose,
      
      LepCorrEff_Trig1L_Medium_isoFixedCutLoose,
      LepCorrEff_Trig1L_Medium_isoFixedCutTight,
      LepCorrEff_Trig1L_Medium_isoGradient,
      LepCorrEff_Trig1L_Medium_isoLoose,
      
      LepCorrEff_Trig1L_Tight_isoFixedCutLoose,
      LepCorrEff_Trig1L_Tight_isoFixedCutTight,
      LepCorrEff_Trig1L_Tight_isoGradient,
      LepCorrEff_Trig1L_Tight_isoLoose,

      PUWeight,
      JVTWeight,
      lepSFObjLoose,
      lepSFObjTight,
      lepSFTrigLoose,
      lepSFTrigTight,
      MV2c20_77_EventWeight,
      MET_RefFinal_et,
      MET_RefFinal_phi,

      MET_PassSelection,
      MET_LCTopo,
      MET_LCTopo_Phi,
      MET_LCTopo_SumEt,
      MET_Track,
      MET_Track_Phi,
      MET_Track_SumEt,

      D0,
      D0Sig,
      Z0,
      Z0Sin,
      Z0Sig,
      nPixHits,
      nPixHoles,
      nPixelOutliers,
      nSCTHits,
      nSCTHoles,
      nSCTOutliers,
      nTRTHits,
      nTRTHoles,
      nTRTOutliers,
      nBLHits,
      nBLayerOutliers,
      expectBLayerHit,
      nIBLHits,
      nIBLayerOutliers,
      expectIBLayerHit,
      chi2,
      ndof,
      qoverp, 
      vx,
      vy,
      vz,
      covariance,
      type,

      HasPrimaryTrackParticle,
      NumberOfPrecisionLayers, 
      MomentumBalanceSignificance,
      MuonType,

      PtCone20,
      PtCone30,
      PtCone40,
      EtCone20,
      EtCone30,
      EtCone40,

      TopoEtCone20,
      TopoEtCone30,
      TopoEtCone40,

      PtVarCone20,
      PtVarCone30,
      PtVarCone40,

      EtaBE2,
      ClEta,
      ClPhi,
      ClEt,

      isLooseBad,
      isTightBad,

      isLooseLH,
      isLooseLHWithBL,
      isMediumLH,
      isTightLH,

      LHLoose,
      LHMedium,
      LHTight,
      LHValue,

      DFCommonElectronsLHLoose,
      DFCommonElectronsLHMedium,
      DFCommonElectronsLHTight,
      
      isoTight,
      isoLooseTrackOnly,
      isoLoose,
      isoGradient,
      isoGradientLoose,
      isoFixedCutTight,
      isoFixedCutTightTrackOnly,
      isoFixedCutLoose,
      
      firstEgMotherPdgId,
      firstEgMotherTruthOrigin,
      firstEgMotherTruthType,
      truthOrigin,
      truthPdgId,
      truthType,
      bkgMotherPdgId,
      bkgTruthOrigin,
      bkgTruthType,
      
      TruthJetDR,
      TruthJetPt,
      TruthJetID,

      JetEMScaleMomentum_phi,
      JetEMScaleMomentum_eta,
      JetEMScaleMomentum_pt,
      JetEMScaleMomentum_m,

      emfrac,
      JVT,
      Jvt,
      SV1,
      IP2D,
      IP3D,
      JetF,
      JetFC,
      DL1,
      DL1mu,
      DL1rnn,
      rnnip,
      MV2c00,
      MV2c10,
      MV2c20,
      MV2c100,
      MV2cl100,
      MV2c10mu,
      MV2c10rnn,
      TileStatus,

      JVTPass,
      JVTPassDefault,
      JVTPassLoose,

      JVTSF_Eff,
      JVTSF_Ineff,
      JVTSF_LooseEff,
      JVTSF_LooseIneff,

      JetBDTSigVeryLoose,
      JetBDTSigLoose,
      JetBDTSigMedium,
      JetBDTSigTight,
      JetBDTScore,

      EleBDTLoose,
      EleBDTMedium,
      EleBDTTight,
      EleBDTScore,

      PassEleOLR,
      MuonVeto,
      DecayMode,

      SumPtTrkPt500,
      NumTrkPt500,
      EMFrac,
      HECQuality,
      LArQuality,
      DetectorEta,
      PartonTruthLabelID,
      ConeTruthLabelID,
      HadronConeExclTruthLabelID,

      NRecoVtx,
      HasPriVtx,
      PriVtxX,
      PriVtxY,
      PriVtxZ,

      NTrack,
      TrackPt,
      TrackEta,
      TrackTheta,
      TrackPhi,
      TrackD0,
      TrackD02D,
      TrackD0Sig,
      TrackZ0Sin,
      TrackZ0Sig,
      TrackNPIX,
      TrackNPIXHole,
      TrackNSCT,
      TrackNSCTHole,
      TrackNIBL,
      TrackNBL,
      TrackExpectIBL,
      TrackExpectBL,
      TrackChi2,
      TrackNdof,
      TrackBarcode,    
     
      AllTrackPt,
      AllTrackEta,
      AllTrackPhi,
      AllTrackD0,
      AllTrackD02D,
      AllTrackD0Sig,
      AllTrackZ0Sin,
      AllTrackZ0Sig,
      AllTrackNPIX,
      AllTrackNPIXHole,
      AllTrackNSCT,
      AllTrackNSCTHole,
      AllTrackNIBL,
      AllTrackNBL,
      AllTrackExpectIBL,
      AllTrackExpectBL,
      AllTrackChi2,
      AllTrackNdof,
      AllTrackBarcode,    

      VeryLoose,
      Loose,
      Medium,
      Tight,
      HighPt,
      Quality,
      IsGoodQuality,
      PassedID,

      //
      // Btagging MVA vars
      //
      pt_uCalib,
      eta_uCalib,
      eta_abs_uCalib,
      truthLabel,
      ip2_c,
      ip2_cu,
      ip3_c,
      ip3_cu,
      ip2,
      ip3,
      sv1_mass,
      sv1_efrc,
      sv1_Lxy,
      sv1_L3d, 
      sv1_sig3, 
      sv1_dR,
      sv1_ntkv,
      sv1_n2t,
      jf_mass,
      jf_efrc,
      jf_dR,
      jf_sig3,
      jf_n2tv,
      jf_ntrkv,
      jf_nvtx,
      jf_nvtx1t
    };
  }
}

#endif
