// -*- c++ -*-
#ifndef ANP_CUTFLOW_H
#define ANP_CUTFLOW_H 

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : CutFlow
 * @Author : Rustem Ospanov
 *
 * @Brief  : Configure and run analysis cut-flow
 * 
 **********************************************************************************/

// C/C++
#include <iostream>
#include <iomanip> 
#include <map>

// Local
#include "CutItem.h"

class TH1;

namespace Ath
{
  class CutFlow
  {
  public:

    CutFlow();
    virtual ~CutFlow() {}
    
    void ConfCuts(const std::vector<std::string> &confs, const std::string &caller, bool print=true);

    template<class T> Cut::State PassCut(const T &event);
    template<class T> Cut::State PassCut(const T &event, double weight);

    void PrintCut (std::ostream &os, const std::string &pad="   ") const { PrintCuts(os, pad); }
    void PrintCuts(std::ostream &os, const std::string &pad="   ") const;

    void PrintConf(std::ostream &os, const std::string &pad="") const;

    TH1* MakeCutFlowHist(const std::string &key) const;

    bool HasCuts() const { return !fCuts.empty(); }

  public:

    struct CutPair
    {
      CutPair():npass(0) {}

      CutItem  icut;

      unsigned npass;
    };

    typedef std::vector<CutPair> CutVec;

  private:

    void AddCut(const std::string &key);

    std::ostream& log() const;

  private:

    bool                        fDebug;
    unsigned                    fNInput;
    
    CutVec                      fCuts;
  };

  //-----------------------------------------------------------------------------
  // Template implementation
  //
  template<class T> inline Cut::State CutFlow::PassCut(const T &event)
  {
    return PassCut(event, 1.0);
  }

  //-----------------------------------------------------------------------------
  template<class T> inline Cut::State CutFlow::PassCut(const T &event, double)
  {
    //
    // Count all events
    //
    ++fNInput;

    if(fCuts.empty()) {
      return Cut::None;
    }

    //
    // Apply cuts
    //
    bool pass = true;
    
    for(CutPair &c: fCuts) {
      if(c.icut.PassCut(event) == Cut::Pass) {
	++c.npass;
      }
      else {
	pass = false;
	break;
      }
    }   

    if(pass) {
      return Cut::Pass;
    }

    return Cut::Fail;
  }
}

#endif
