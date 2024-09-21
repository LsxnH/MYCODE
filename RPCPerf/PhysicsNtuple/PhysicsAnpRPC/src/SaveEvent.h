// -*- c++ -*-
#ifndef ATH_SAVEEVENT_H
#define ATH_SAVEEVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : ReadJets
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 * Top level algorithm for processing tools for filling ntuples
 * 
 **********************************************************************************/

// ROOT
#include "TStopwatch.h"

// Framework
#include "GaudiKernel/ITHistSvc.h"

// Athena
#include "AthenaBaseComps/AthAlgorithm.h"

// Local
#include "IEventTool.h"
#include "WriteEvent.h"

class TTree;
class TFile;

namespace Ath
{
  class SaveEvent : virtual public AthAlgorithm
  {
  public:
    
    SaveEvent(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~SaveEvent() {}
    
    StatusCode initialize();

    StatusCode execute(); 

    StatusCode finalize();
    
  private:
    
    TTree* MakeTree(const std::string &tname, const std::string &sname);
    
    void RegHist(TH1 *h, const std::string &key);   
    
  private:
    
    // Properties:
    std::string                         m_outputStream;     // Output THist stream name
    std::string                         m_outputFile;       // Output ROOT file
    
    // Tools and services:
    ServiceHandle<ITHistSvc>            m_histSvc;          // THist handle
    
    ToolHandle<Ath::IEventTool>         m_writeEvent;       // Tool to write out event
    
    // Variables:
    TStopwatch                          m_timerEvent;       // Total job timer
    TStopwatch                          m_timerTotal;       // Total job timer
    
    int                                 m_countEvent;
    
    TFile                              *m_file;
    TTree                              *m_treeEvent;        // Output event tree
  };
}
#endif
