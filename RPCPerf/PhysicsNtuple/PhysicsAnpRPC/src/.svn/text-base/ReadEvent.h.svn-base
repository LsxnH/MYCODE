// This is -*- c++ -*-
#ifndef ATH_READEVENT_H
#define ATH_READEVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : ReadEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Algorithm to read event info and create local event object
 * 
 **********************************************************************************/
//
// Algorithm for processing tools for filling ntuples
//

// ROOT
#include "TStopwatch.h"

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// Athena 
#include "AthenaBaseComps/AthAlgorithm.h"
#include "TrigDecisionTool/TrigDecisionTool.h"

class TH1;

namespace Ath
{
  class ReadEvent : virtual public AthAlgorithm
  {
  public:
    
    ReadEvent(const std::string& name, ISvcLocator* pSvcLocator);
    virtual ~ReadEvent() {}
    
    StatusCode initialize();
    StatusCode execute(); 
    StatusCode finalize();
    
  private:
    
    void RegHist(TH1 *h, const std::string &key);   
    
  private:
    
    // Properties:
    std::string                             m_outputStream;
    std::string                             m_outputTruthVtxName;
    std::string                             m_outputTruthPartName;
    
    std::vector<std::string>                m_triggers;
    std::vector<std::string>                m_dummyStrs;

    bool                                    m_debug;
    bool                                    m_doTrigDecision;

    // Tools and services:
    ServiceHandle<ITHistSvc>                m_histSvc;
    
    ToolHandle<Trig::TrigDecisionTool>      m_trigDecision;

    // Variables:
    TStopwatch                              m_timerEvent;
    TStopwatch                              m_timerTotal;
    TStopwatch                              m_timerTrigger;
    
    int                                     m_countEvent;
  };
}

#endif
