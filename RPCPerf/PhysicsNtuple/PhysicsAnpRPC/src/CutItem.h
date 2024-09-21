// -*- c++ -*-
#ifndef ANP_CUTITEM_H
#define ANP_CUTITEM_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : CutItem
 * @Author : Rustem Ospanov
 *
 * @Brief  : Class to configure and apply cuts
 * 
 *  CutItem is a helper class:
 *    - read cut definitions from Registry
 *    - check if variable passes cut
 * 
 **********************************************************************************/

// C/C++
#include <cmath>
#include <iostream>
#include <string>

namespace Ath
{
  //
  // Enum for event selection
  //
  namespace Cut
  {
    enum State { Fail=0, Pass=1, None=2 };
  }

  class CutItem
  {
  public:

    CutItem();
    ~CutItem() {}

    bool InitCut(const std::string &conf);

    template<class T> Cut::State PassCut(const T &event);

    const std::string& GetName() const { return fCutName; }
    const std::string& GetConf() const { return fCutConf; }
          std::string& GetConf()       { return fCutConf; }
    
    void PrintConfig(std::ostream &os = std::cout, const std::string &pad = "") const;

    std::string ConfigAsString(const std::string &pad) const;
    
  public:
   
    enum Compare { None, Less, LessOrEqual, Greater, GreaterOrEqual, Equal, NotEqual };

    struct ComparePoint
    {
      ComparePoint() :use_abs(false), compare(None), cut(0.0), key(0) {}

      template<class T> Cut::State Pass(const T &vars) const;

      bool          use_abs;
      Compare       compare;
      double        cut;
      uint32_t      key;

      std::string   opr;
      std::string   var;
    };

    typedef std::vector<ComparePoint>  ComVec;
  
    static std::string GetString(Compare comp, bool print_nice=true);

    static const std::vector<Ath::CutItem::Compare>& GetAllComp();

  private:

    std::ostream& log() const;

    bool AddComparePoint(const std::string &comp, bool isOR);
    
    Compare GetCompare(const std::string &val) const;
    
    std::string StripSpaces(const std::string &s) const;

    std::pair<uint32_t, bool> ParseKey(const std::string &key) const;
    
  private:  
    
    //
    // Configuration
    //
    std::string  fCutName;
    std::string  fCutConf;

    bool         fDebug;
    bool         fDummy;

    ComVec       fCompA;
    ComVec       fCompO;
  };
  
  //-----------------------------------------------------------------------------
  // Inlined member functions
  //
  template<class T> inline Cut::State CutItem::ComparePoint::Pass(const T &event) const 
  {
    double val = 0.0;
    
    if(!event.GetVar(key, val)) {
      return Cut::None;
    }
    
    if(use_abs) {
      val = std::fabs(val);
      //std::cout << "opr=" << opr << "  var=" << var << "  val=" << val << std::endl;
    }       
    
    Cut::State result = Cut::Fail;
    
    if     (compare == Less            &&   val < cut  )                 result = Cut::Pass;
    else if(compare == LessOrEqual     && !(val > cut) )                 result = Cut::Pass;
    else if(compare == Greater         &&   val > cut  )                 result = Cut::Pass;
    else if(compare == GreaterOrEqual  && !(val < cut) )                 result = Cut::Pass;
    else if(compare == Equal           && !(val < cut) && !(val > cut) ) result = Cut::Pass;
    else if(compare == NotEqual        &&  (val < cut  ||   val > cut) ) result = Cut::Pass;
    
    return result;
  }
  
  //-----------------------------------------------------------------------------
  // Check if value passes cut
  //
  template<class T> inline Cut::State CutItem::PassCut(const T &event) 
  {
    if(fDummy) {
      return Cut::Pass;
    }

    Cut::State pass = Cut::None;
    
    if(!fCompA.empty()) {
      pass = Cut::Pass;

      for(ComVec::const_iterator cit = fCompA.begin(); cit != fCompA.end(); ++cit) {
	if(cit->Pass(event) != Cut::Pass) {
	  pass = Cut::Fail;
	  break;
	}
      }
    }
    else if(!fCompO.empty()) {
      pass = Cut::Fail;

      for(ComVec::const_iterator cit = fCompO.begin(); cit != fCompO.end(); ++cit) {
	if(cit->Pass(event) == Cut::Pass) {
	  pass = Cut::Pass;
	  break;
	}
      }
    }

    return pass;
  }
}

#endif
