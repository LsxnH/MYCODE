
// ROOT
#include "TH1.h"
#include "TTree.h"

// Local
#include "SaveEvent.h"

// Local
#include "Event.h"
#include "VarDefs.h"

using namespace Ath;

//=============================================================================
SaveEvent::SaveEvent(const std::string& name,
			 ISvcLocator* pSvcLocator)
  :AthAlgorithm(name, pSvcLocator),
   m_histSvc   ("THistSvc/THistSvc", name),
   m_countEvent(0),
   m_file      (0),
   m_treeEvent (0)
{
  declareProperty("writeEvent",    m_writeEvent);

  declareProperty("outputStream",  m_outputStream, "Output stream name");
  declareProperty("outputFile",    m_outputFile,   "Output file name");
}

//=============================================================================
StatusCode SaveEvent::initialize()
{    
  //
  // Initialize tools and services
  //
  CHECK( m_histSvc   .retrieve() );
  CHECK( m_writeEvent.retrieve() );

  //
  // Start total 
  //
  m_timerTotal.Start();

  msg(MSG::INFO) << "Print all known enums" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode SaveEvent::execute()
{
  //
  // Process current event
  //
  msg(MSG::DEBUG) << "execute() - begin..." << endmsg;

  m_countEvent++;
  
  DataHandle<Ath::Event> evt_handle;
  if(evtStore() -> retrieve(evt_handle).isFailure() || !evt_handle.cptr()) {
    msg() << MSG::ERROR << "Faied to get Ath::Event" << endmsg;
    return StatusCode::FAILURE;
  }

  msg(MSG::DEBUG) << "Retrieved Ath::Event handle" << endmsg;

  m_writeEvent->ProcEvent(*evt_handle);

  msg(MSG::DEBUG) << "execute() - finished" << endmsg;  
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode SaveEvent::finalize()
{
  //
  // Finalize output
  //
  Def::PrintVarNames();

  return StatusCode::SUCCESS;
}

//=============================================================================
TTree* SaveEvent::MakeTree(const std::string &tree_name, 
			   const std::string &stream_name)
{
  //
  // Make and register TTree via THistSvc
  //
  if(tree_name.empty() || stream_name.empty()) { 
    return 0;
  }

  const std::string tree_key = "/"+stream_name+"/"+tree_name;  
  TTree *tree = 0;

  if(m_histSvc->exists(tree_key) && m_histSvc->getTree(tree_key, tree).isSuccess()) {
    msg() << MSG::INFO << "Retrieved TTree from HistSvc: " << tree_key << endmsg;    
    return tree;
  }

  tree = new TTree(tree_name.c_str(), tree_name.c_str());
  tree -> SetDirectory(0);
  
  if(m_histSvc -> regTree(tree_key, tree).isFailure()) {
    msg() << MSG::WARNING << "Could not register TTree " << tree_name << endmsg;
    delete tree; tree = 0;
  }
  else {
    msg() << MSG::INFO << "Registered new TTree " << tree_key << endmsg;
  }

  return tree;
}

//=============================================================================
void SaveEvent::RegHist(TH1 *h, const std::string &key)
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
