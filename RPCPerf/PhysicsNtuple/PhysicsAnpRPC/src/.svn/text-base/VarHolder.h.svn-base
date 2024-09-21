// -*- c++ -*-
#ifndef ATH_VARHOLDER_H
#define ATH_VARHOLDER_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : VarHolder
 * @Author : Rustem Ospanov
 *
 * @Brief  : VarHolder is candidate analysis object created by algorithms
 *  
 **********************************************************************************/

// C/C++
#include <algorithm>
#include <iostream>

// Local
#include "VarEntry.h"
#include "VecEntry.h"

namespace Ath
{
  class VarHolder 
  {
  public:

    VarHolder() {}
    virtual ~VarHolder() {}
    
    static std::string GetObjectType() { return "VarHolder"; }
    
    bool  ReplaceVar(unsigned key, double value);
    bool  AddVar    (unsigned key, double value, Type type=kNone);
    bool  AddVec    (unsigned key, const std::vector<int>   &vec);
    bool  AddVec    (unsigned key, const std::vector<float> &vec);
    bool  DelVar    (unsigned key);

    double GetVar   (unsigned key) const;
    bool   GetVar   (unsigned key, double             &value) const;
    bool   GetVar   (unsigned key, float              &value) const;
    bool   GetVarVec(unsigned key, std::vector<int>   &value) const;
    bool   GetVarVec(unsigned key, std::vector<float> &value) const;

    bool  HasKey(unsigned key) const;
    bool  HasVar(unsigned key) const;

          VarEntryVec& GetVars    ()       { return fVars; }
    const VarEntryVec& GetVars    () const { return fVars; }
    const VarEntryVec& GetVarEntry() const { return fVars; }

    const std::vector<VecEntry<int> >&   GetIntVecs  () const { return fInts;   }
    const std::vector<VecEntry<float> >& GetFloatVecs() const { return fFloats; }

    virtual void ClearVars();

  private:

    VarEntryVec                   fVars;
    std::vector<VecEntry<int> >   fInts;
    std::vector<VecEntry<float> > fFloats;
  };
  
  //
  // Inlined functions
  //
  inline bool VarHolder::ReplaceVar(const unsigned key, const double value)
  {
    if(!HasKey(key)) {
      fVars.push_back(VarEntry(key, value));
      return true;
    }
    else{
      DelVar(key);
      AddVar(key, value);
    }

    return false;
  }
  
  inline bool VarHolder::AddVar(const unsigned key, const double value, Type type)
  {
    if(!HasKey(key)) {
      fVars.push_back(VarEntry(key, value, type));
      return true;
    }
    
    std::cout << GetObjectType() << "::AddVar(" << key << ", " << value << ") - key already exists" << std::endl;
    return false;
  }

  inline bool VarHolder::AddVec(const unsigned key, const std::vector<int> &vec)
  {
    if(!HasKey(key)) {
      fInts.push_back(VecEntry<int>(key, vec));
      return true;
    }
    
    std::cout << GetObjectType() << "::AddVec(" << key << ") - key already exists" << std::endl;
    return false;
  }

  inline bool VarHolder::AddVec(const unsigned key, const std::vector<float> &vec)
  {
    if(!HasKey(key)) {
      fFloats.push_back(VecEntry<float>(key, vec));
      return true;
    }
    
    std::cout << GetObjectType() << "::AddVec(" << key << ") - key already exists" << std::endl;
    return false;
  }
  
  inline bool VarHolder::DelVar(const unsigned key)
  {
    VarEntryVec::iterator vit = fVars.begin();
    while(vit != fVars.end()) {
      if(vit->GetKey() == key) {
	vit = fVars.erase(vit);
      }
      else {
	vit++;
	}
    }
    
    return false;
  }
  
  inline bool VarHolder::HasKey(unsigned key) const
  {
    return std::find(fVars.begin(), fVars.end(), key) != fVars.end() 
      || std::find(fInts  .begin(), fInts  .end(), key) != fInts  .end()
	|| std::find(fFloats.begin(), fFloats.end(), key) != fFloats.end();
  }

  inline bool VarHolder::HasVar(unsigned key) const
  {
    return std::find(fVars.begin(), fVars.end(), key) != fVars.end();
  }
  
  inline bool VarHolder::GetVar(unsigned key, float &value) const
  {
    //
    // Read variable
    //
    const VarEntryVec::const_iterator ivar = std::find(fVars.begin(), fVars.end(), key);
    if(ivar != fVars.end()) {
      value = ivar->GetData();
      return true;
    }
    
    return false;
  }
  
  inline bool VarHolder::GetVar(unsigned key, double &value) const
  {
    //
    // Read variable
    //
    const VarEntryVec::const_iterator ivar = std::find(fVars.begin(), fVars.end(), key);
    if(ivar != fVars.end()) {
      value = ivar->GetData();
      return true;
    }
    
    return false;
  }
  
  inline bool VarHolder::GetVarVec(unsigned key, std::vector<int> &value) const
  {
    //
    // Read variable
    //
    const std::vector<VecEntry<int> >::const_iterator ivar = std::find(fInts.begin(), fInts.end(), key);
    if(ivar != fInts.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }
  
  inline bool VarHolder::GetVarVec(unsigned key, std::vector<float> &value) const
  {
    //
    // Read variable
    //
    const std::vector<VecEntry<float> >::const_iterator ivar = std::find(fFloats.begin(), fFloats.end(), key);
    if(ivar != fFloats.end()) {
      value = ivar->GetVec();
      return true;
    }
    
    return false;
  }
  
  inline double VarHolder::GetVar(const unsigned key) const
  {
    //
    // Find and return, if exists, value stored at key
    //
    double val = -1.0e9;
    GetVar(key, val);    
    return val;
  }

  inline void VarHolder::ClearVars()
  {
    fVars  .clear();
    fInts  .clear();
    fFloats.clear();
  }
}

#endif
