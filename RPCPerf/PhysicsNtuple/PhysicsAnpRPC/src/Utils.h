// -*- c++ -*-
#ifndef ATH_UTIL_H
#define ATH_UTIL_H

// C/C++
#include <iostream>
#include <string>
#include <vector>

// ROOT
#include "TStopwatch.h"

// Athena
#include "GaudiKernel/IAlgTool.h"

// xAOD
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "xAODTruth/TruthVertexContainer.h"

// Local
#include "Object.h"
#include "VarDefs.h"
#include "VarHolder.h"

namespace Ath
{
  class Event;

  void StringTok(std::vector<std::string>& ls,
		 const std::string& str,
		 const std::string& tok);

  std::string PadStrFront(std::string str, int width);
  std::string PadStrBack (std::string str, int width);

  bool AddMom(VarHolder &h, const xAOD::IParticle &p);

  bool AddImpactParameters(VarHolder &h, const xAOD::TrackParticle *track, const Event &event);

  std::string PrintResetStopWatch(TStopwatch &watch);

  uint32_t getTrackBarcode(const xAOD::TrackParticle *track);

  void resetTrackBarcodes();

  std::string PrintAuxVars(const xAOD::IParticle &p, const std::string &prefix);

  std::string ParticleAsStr(const xAOD::TruthParticle &part);

  int GetInt(const Ath::VarHolder &p, uint32_t var);

  bool IsGeneratorParticle(const xAOD::TruthParticle &truth);

  bool MatchPdgId(const xAOD::TruthParticle &truth, const std::vector<int> &pdgids);

  void FillTruthParticle(Ath::Object &p, const xAOD::TruthParticle &part);
  void FillTruthVertex  (Ath::Object &v, const xAOD::TruthVertex   &vtx);

  void AddTruthParents (const xAOD::TruthParticle &part, Ath::Object &p);
  void AddTruthChildren(const xAOD::TruthParticle &part, Ath::Object &p);

  std::vector<const xAOD::TruthParticle *> GetTruthParents    (const xAOD::TruthParticle &part);
  std::vector<const xAOD::TruthParticle *> GetTruthChildren   (const xAOD::TruthParticle &part);
  std::vector<const xAOD::TruthParticle *> GetAllTruthChildren(const xAOD::TruthParticle &part);

  std::string PrintToolProperties(IAlgTool *ialg);

  class TimerScopeHelper
  {
  public:
    
    explicit TimerScopeHelper(TStopwatch &timer) 
      :fTimer(timer) { fTimer.Start(false); }
    ~TimerScopeHelper() { fTimer.Stop(); }
    
  private:
      
    TStopwatch &fTimer;
  };
}

#endif
