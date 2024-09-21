// -*- c++ -*-
#ifndef ATH_WRITEUTILS_H
#define ATH_WRITEUTILS_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : WriteEvent
 * @Author : Rustem Ospanov
 *
 * @Brief  : Common helper code to write out flat ntuples
 * 
 **********************************************************************************/

// C/C++
#include <map>

// Local
#include "IEventTool.h"
#include "VarDefs.h"
#include "VarEntry.h"

class TBranch;
class TTree;

namespace Ath
{  
  class Event;
  
  Type        GetDataType(std::string type);
  std::string GetDataNick(std::string type);

  //------------------------------------------------------------------------
  struct VarConfig
  {
    VarConfig():debug(false), var(Def::NONE), type(kNone), default_value(-9999.0) {}

    bool ConfigVarFromStr(const std::string &config);

    bool ReadValueFromStr(const std::string &input, std::string &variable, std::string &value);

    std::string RemoveWhiteSpaces(std::string str);

    std::string PrintVarConfig() const;

    bool        debug;
    uint32_t    var;
    Type        type;
    double      default_value;

    std::string conf;
    std::string name;
    std::string nick;
  };

  //------------------------------------------------------------------------
  struct Data 
  {
    Data();
    
    bool InitFromVarEntry (const VarEntry  &var);
    bool InitFromVarConfig(const VarConfig &var);
    
    void ClearVar();
    
    void ClearVec();
    
    template<class T> bool ReadVar(const T &obj);
    
    void FillVec();
    
    TBranch* BookVarBranch(TTree *tree);
    TBranch* BookVecBranch(TTree *tree, const std::string &prefix);
    
    Float_t               val_flt;
    Double_t              val_dbl;
    Int_t                 val_int;
    Short_t               val_snt;
    Long64_t              val_lnt;
    UInt_t                val_unt;
    uint64_t              val_uln;
    Bool_t                val_bln;

    std::vector<Float_t>  vvl_flt;
    std::vector<Int_t>    vvl_int;
    
    std::vector<Float_t>   *vec_flt;
    std::vector<Double_t>  *vec_dbl;
    std::vector<Int_t>     *vec_int;
    std::vector<Short_t>   *vec_snt;
    std::vector<Long64_t>  *vec_lnt;
    std::vector<UInt_t>    *vec_unt;
    std::vector<uint64_t>  *vec_uln;
    std::vector<Bool_t>    *vec_bln;

    std::vector<std::vector<Float_t> >  *vc2_flt;
    std::vector<std::vector<Int_t> >    *vc2_int;
    
    std::string name;
    std::string nick;

    uint32_t    var;
    Type        type;
  };

  typedef std::vector<Data> DataVec;
  
  //------------------------------------------------------------------------
  // Inlined functions
  //
  inline void Data::ClearVar() 
  {
    val_flt = -1000.0;
    val_dbl = -1000.0;
    val_int = -1000;
    val_snt = -1000;
    val_lnt = -1000;
    val_unt = 0;
    val_uln = 0;
    val_bln = false;
    
    vvl_flt.clear();
    vvl_int.clear();
  }

  inline void Data::ClearVec() 
  {
    if(vec_flt) vec_flt->clear();
    if(vec_dbl) vec_dbl->clear();
    if(vec_int) vec_int->clear();
    if(vec_snt) vec_snt->clear();
    if(vec_lnt) vec_lnt->clear();
    if(vec_unt) vec_unt->clear();
    if(vec_uln) vec_uln->clear();
    if(vec_bln) vec_bln->clear();

    if(vc2_flt) vc2_flt->clear();
    if(vc2_int) vc2_int->clear();
  }
  
  template<class T> bool Data::ReadVar(const T &obj)
  {
    ClearVar();
    double val = 0.0;

    if(type == kVecFloat) {
      if(!obj.GetVarVec(var, vvl_flt)) {
	std::cout << "ReadVar - line " << __LINE__ << " - missing var=" << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
	return false;
      }
    }    
    else if(type == kVecInt) {
      if(!obj.GetVarVec(var, vvl_int)) {
	std::cout << "ReadVar - line " << __LINE__ << " - missing var=" << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
	return false;
      }
    } 
    else if(!obj.GetVar(var, val)) {
      std::cout << "ReadVar - line " << __LINE__ << " - missing var=" << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
      return false;
    }

    val_flt = val;
    val_dbl = val;

    if(false) {
      std::cout << "ReadVar - var=" << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << " val=" << val << std::endl;
    }
      
    if(type == kInt) {
      if(std::fabs(val) < double(INT_MAX)) {
	val_int = static_cast<Int_t>(val);
      }
      else {
	std::cout << "ReadVar<" << obj.GetObjectType() << "> - overflow error: var=" 
		  << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
      }
    }    
    else if(type == kShort) {
      if(std::fabs(val) < double(SHRT_MAX)) {
	val_snt = static_cast<Short_t>(val);
      }
      else {
	std::cout << "ReadVar<" << obj.GetObjectType() << "> - overflow error: var=" 
		    << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
	return false;
      }
    }    
    else if(type == kBool) {
      if(val > 0.0) { val_bln = true;  }
      else          { val_bln = false; }
    }
    else if(type == kUInt) {
      if(std::fabs(val) < double(UINT_MAX)) {
	val_unt = static_cast<UInt_t>(val);
      }
      else {
	std::cout << "ReadVar<" << obj.GetObjectType() << "> - overflow error: var=" 
		    << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
	return false;
      }
    }
    else if(type == kLong) {
      if(std::fabs(val) < double(LONG_MAX)) {
	val_lnt = static_cast<Long_t>(val);
      }
      else {
	std::cout << "ReadVar<" << obj.GetObjectType() << "> - overflow error: var=" 
		    << Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
	return false;
      }
    }
    else if(type == kULong) {
      std::cout << "ReadVar<" << obj.GetObjectType() << "> - not implemented: var=" 
		<< Def::AsStr(var) << " name=" << name << " type=" << GetTypeAsStr(type) << std::endl;
      return false;
    }

    return true;
  }
  
  inline void Data::FillVec() 
  {	
    if     (vec_flt) { vec_flt->push_back(val_flt); }
    else if(vec_dbl) { vec_dbl->push_back(val_dbl); }
    else if(vec_int) { vec_int->push_back(val_int); }
    else if(vec_snt) { vec_snt->push_back(val_snt); }
    else if(vec_lnt) { vec_lnt->push_back(val_lnt); }
    else if(vec_unt) { vec_unt->push_back(val_unt); }
    else if(vec_uln) { vec_uln->push_back(val_uln); }
    else if(vec_bln) { vec_bln->push_back(val_bln); }	
    else if(vc2_flt) { vc2_flt->push_back(vvl_flt); }
    else if(vc2_int) { vc2_int->push_back(vvl_int); }
  }

  //=============================================================================
  // Template for extracting variables
  //=============================================================================
  template<class T> void ReadVecData(bool read, 
				     std::vector<Data> &dvec, 
				     const std::vector<T> &objs)
  {
    //
    // Read object data
    //
    if(!read) return;
    
    for(const T &obj: objs) {
      for(Data &data: dvec) {
	data.ReadVar(obj);		
	data.FillVec();
      }
    }
  }

  //=============================================================================
  // Helper functions
  //=============================================================================
  bool HasKeyInDataVec(const DataVec &dvec, unsigned key);
}

#endif
