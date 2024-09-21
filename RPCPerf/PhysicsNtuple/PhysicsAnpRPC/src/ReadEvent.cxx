// Local
#include "VarDefs.h"
#include "Event.h"
#include "Utils.h"
#include "ReadEvent.h"

// Athena
#include "xAODEventInfo/EventInfo.h"
#include "xAODTracking/VertexContainer.h"

// ROOT
#include "TH1.h"

using namespace Ath;

//=============================================================================
ReadEvent::ReadEvent(const std::string& name,
		     ISvcLocator* pSvcLocator)
  :AthAlgorithm(name, pSvcLocator),
   m_histSvc   ("THistSvc/THistSvc", name),
   m_countEvent(0)
{
  declareProperty("outputStream",        m_outputStream);

  declareProperty("debug",               m_debug          = false);
  declareProperty("doTrigDecision",      m_doTrigDecision = false);
  declareProperty("triggers",            m_triggers);

  declareProperty("trigDecision",        m_trigDecision);
}

//=============================================================================
StatusCode ReadEvent::initialize()
{    
  //
  // Initialize tools and services
  //
  CHECK( m_histSvc.retrieve() );

  if(m_doTrigDecision) {
    CHECK( m_trigDecision.retrieve() );
  }

  m_timerTotal  .Reset();
  m_timerEvent  .Reset();
  m_timerTrigger.Reset();

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode ReadEvent::execute()
{
  //
  // Process current event
  //
  TimerScopeHelper timer(m_timerEvent);

  if(m_countEvent == 0) {
    m_timerTotal.Start();    
  }

  m_countEvent++;
  
  const DataHandle<xAOD::EventInfo> event_handle;
  CHECK( evtStore()->retrieve(event_handle) );

  Ath::Event *event = new Ath::Event();
  CHECK( evtStore()->record(event, SG::DEFAULTKEY, true) );

  //-----------------------------------------------------------------------------
  // Process event info
  //  
  event->SetRunNumber     (event_handle->runNumber  ());
  event->SetEventNumber   (event_handle->eventNumber());
  event->SetSimulationFlag(event_handle->eventType(xAOD::EventInfo::IS_SIMULATION));
  event->SetBeamPosSigmaX (event_handle->beamPosSigmaX ());
  event->SetBeamPosSigmaY (event_handle->beamPosSigmaY ());
  event->SetBeamPosSigmaXY(event_handle->beamPosSigmaXY());

  event->AddVar(Ath::Def::BeamPosX,              event_handle->beamPosX());
  event->AddVar(Ath::Def::BeamPosY,              event_handle->beamPosY());
  event->AddVar(Ath::Def::BeamPosZ,              event_handle->beamPosZ());

  event->AddVar(Ath::Def::BeamPosSigmaX,         event_handle->beamPosSigmaX());
  event->AddVar(Ath::Def::BeamPosSigmaY,         event_handle->beamPosSigmaY());
  event->AddVar(Ath::Def::BeamPosSigmaZ,         event_handle->beamPosSigmaZ());

  event->AddVar(Ath::Def::Event,                 event_handle->eventNumber());
  event->AddVar(Ath::Def::Run,                   event_handle->runNumber());
  event->AddVar(Ath::Def::LumiBlock,             event_handle->lumiBlock());
  event->AddVar(Ath::Def::bcid,                  event_handle->bcid());
  event->AddVar(Ath::Def::backgroundFlags,       event_handle->eventFlags(xAOD::EventInfo::Background));
  event->AddVar(Ath::Def::ActualInteractions,    event_handle->actualInteractionsPerCrossing());
  event->AddVar(Ath::Def::AverageInteractions,   event_handle->averageInteractionsPerCrossing());

  event->AddVar(Ath::Def::EventFlag_Core,        event_handle->eventFlags(xAOD::EventInfo::Core) & 0x40000);  
  event->AddVar(Ath::Def::ErrorState_Core,       event_handle->errorState(xAOD::EventInfo::Core)      );
  event->AddVar(Ath::Def::ErrorState_Background, event_handle->errorState(xAOD::EventInfo::Background));
  event->AddVar(Ath::Def::ErrorState_LAr,        event_handle->errorState(xAOD::EventInfo::LAr)       );
  event->AddVar(Ath::Def::ErrorState_Tile,       event_handle->errorState(xAOD::EventInfo::Tile)      );
  event->AddVar(Ath::Def::ErrorState_SCT,        event_handle->errorState(xAOD::EventInfo::SCT)       );
  event->AddVar(Ath::Def::ErrorState_Pixel,      event_handle->errorState(xAOD::EventInfo::Pixel)     );
  event->AddVar(Ath::Def::ErrorState_TRT,        event_handle->errorState(xAOD::EventInfo::TRT)       );
  event->AddVar(Ath::Def::ErrorState_Muon,       event_handle->errorState(xAOD::EventInfo::Muon)      );

  if(event_handle->eventType(xAOD::EventInfo::IS_SIMULATION)) {
    event->AddVar(Ath::Def::MCChannel,      event_handle->mcChannelNumber());
    event->AddVar(Ath::Def::MCWeight,       event_handle->mcEventWeight());
    event->AddVar(Ath::Def::SimulationType, 0.0);

    event->SetMCChannel(event_handle->mcChannelNumber());
    event->SetMCWeight (event_handle->mcEventWeight ());
  }
  else {
    event->AddVar(Ath::Def::MCChannel,       0);
    event->AddVar(Ath::Def::MCWeight,        0.0);
    event->AddVar(Ath::Def::SimulationType,  0.0);

    event->SetMCChannel(0);
    event->SetMCWeight (0.0);
  }
  
  if(m_doTrigDecision) {
    TimerScopeHelper timer(m_timerTrigger);
  
    for(const std::string &trigger: m_triggers) {      
      const uint32_t tvar = Def::RegisterDynamicVar(trigger);
      const bool     pass = m_trigDecision->isPassed(trigger);
      
      event->AddVar(tvar, pass, Ath::kShort);
      
      if(m_debug) {
	msg(MSG::DEBUG) << trigger << " pass=" << pass << " var=" << tvar << endmsg;
      }
    }
  }

  //---------------------------------------------------------------------------------
  // Retrieve vertex container
  //
  const xAOD::VertexContainer* vtxContainer = 0;
  CHECK(evtStore() -> retrieve(vtxContainer, "PrimaryVertices") );

  const xAOD::Vertex *privtx = 0;

  for(const xAOD::Vertex *vtx: *vtxContainer) {
    if(vtx->vertexType() == xAOD::VxType::PriVtx) {
      privtx = vtx;
      break;
    }
  }
  
  if(privtx) {
    event->AddVar(Ath::Def::HasPriVtx, 1.0);
    event->AddVar(Ath::Def::PriVtxZ,   privtx->z());
    event->AddVar(Ath::Def::NRecoVtx,  vtxContainer->size());
    
    event->SetPriVtxZ      (privtx->z());
    event->SetPriVertexFlag(true);
  }
  else {
    event->AddVar(Ath::Def::HasPriVtx, 0.0);
    event->AddVar(Ath::Def::PriVtxZ,   1.0e9);
    event->AddVar(Ath::Def::NRecoVtx,  0.0);
  
    event->SetPriVtxZ      (1.0e9);
    event->SetPriVertexFlag(false); 
  }

  if(m_debug) {
    std::cout 
      << "execute() - new event" << std::endl
      << "  run:            " << event_handle->eventNumber()      << std::endl
      << "  lumi:           " << event_handle->lumiBlock()        << std::endl
      << "  event:          " << event_handle->eventNumber()      << std::endl
      << "  MC channel:     " << event       ->GetMCChannel()     << std::endl
      << "  MC weight:      " << event       ->GetMCWeight()      << std::endl
      << "  beamPosSigmaX:  " << event_handle->beamPosSigmaX()    << std::endl
      << "  beamPosSigmaY:  " << event_handle->beamPosSigmaY()    << std::endl
      << "  beamPosSigmaXY: " << event_handle->beamPosSigmaXY()   << std::endl
      << "  IsSimuation:    " << event_handle->eventType(xAOD::EventInfo::IS_SIMULATION) << std::endl
      << "  Number of vars: " << event->GetVars().size()         << std::endl;

    for(const VarEntry &var: event->GetVars()) {
      std::cout << "   " << std::setw(10) << std::left << var.GetKey() 
		<< "   " << std::setw(40) << std::left << Def::AsStr(var.GetKey()) 
		<< " = " << var.GetValue()
		<< std::endl;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode ReadEvent::finalize()
{
  //
  // Finalize output
  //
  msg(MSG::INFO) << " processed " << m_countEvent << " event(s)" << endmsg;

  std::cout << "ReadEvent - timer total:   " << PrintResetStopWatch(m_timerTotal)   << std::endl;
  std::cout << "ReadEvent - timer execute: " << PrintResetStopWatch(m_timerEvent)   << std::endl;
  std::cout << "ReadEvent - timer trigger: " << PrintResetStopWatch(m_timerTrigger) << std::endl;

  return StatusCode::SUCCESS;
}

//=============================================================================
void ReadEvent::RegHist(TH1 *h, const std::string &key)
{
  //
  // Register histogram
  //
  std::string hist_key = "/"+m_outputStream+"/"+key;
  if(key.empty() && h) {
    hist_key = "/"+m_outputStream+"/"+std::string(h->GetName());
  }
  
  if(m_histSvc->regHist(hist_key, h).isFailure()) {
    msg() << MSG::WARNING << "Could not register histogram: " << hist_key << endmsg;
  }
  else {
    msg() << MSG::INFO << "Registered histogram: " << hist_key << endmsg;
  }
}
