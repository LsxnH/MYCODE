// -*- c++ -*-
#ifndef ATH_WRITEEVENT_H
#define ATH_WRITEEVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : WriteEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : write Event as flat ntuple
 * 
 **********************************************************************************/

// C/C++
#include <map>

// Gaudi
#include "GaudiKernel/ITHistSvc.h"

// Athena
#include "AsgTools/AsgMetadataTool.h"

// Local
#include "IEventTool.h"
#include "WriteUtils.h"

// Local
#include "VarDefs.h"
#include "VarHolder.h"

class TBranch;
class TTree;

namespace Ath
{  
  static const InterfaceID IID_WriteEvent("Ath::WriteEvent", 1, 0);

  class WriteEvent: virtual public asg::AsgMetadataTool, virtual public IEventTool
  {
  public:

    ASG_TOOL_CLASS( WriteEvent, IEventTool )

    WriteEvent(const std::string &name);
    virtual ~WriteEvent() {}
    
    StatusCode initialize();

    StatusCode beginInputFile();

    StatusCode finalize();

    void ProcEvent(Ath::Event &event);

  private:

    void InitEvent(Ath::Event &event);

    void BookBranches();

    void Run(Event &event);

    void ReadKeys(std::vector<Data> &dvec, const std::string &val) const;

    void PrintKeys(const std::vector<Data> &dvec, const std::string &name);

    TTree* MakeTree(const std::string &tname, const std::string &sname);

    void RegHist(TH1 *h, const std::string &key);

  private:

    typedef std::map<std::string, DataVec> BranchMap;
    typedef std::vector<std::string> StrVec;

  private:

    ServiceHandle<ITHistSvc>  m_histSvc;

    // Properties:    
    bool             m_debug;
    bool             m_print;
    bool             m_doDynamicVarConfig;

    std::string      m_keys_event;
    std::string      m_stream;
    std::string      m_treename;

    StrVec           m_branches;

    // Variables:
    TTree           *m_tree;
    ULong64_t        m_PRWHash;

    bool             m_doFirstEventConfig;

    double           m_inputSumEventWeight;
    double           m_inputSumEventWeight2;
    unsigned         m_inputSumEvent;

    double           m_writeSumEventWeight;
    double           m_writeSumEventWeight2;
    unsigned         m_writeSumEvent;

    BranchMap        m_branchMap;

    DataVec          m_data_event;
  };
}

#endif
