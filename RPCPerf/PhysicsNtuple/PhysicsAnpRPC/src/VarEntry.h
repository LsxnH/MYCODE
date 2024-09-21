// This is -*- c++ -*-
#ifndef ATH_VARENTRY_H
#define ATH_VARENTRY_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : VarEntry
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 * One floating variable with unsigned integer key
 *
 **********************************************************************************/

// C/C++
#include <stdint.h>
#include <iostream>
#include <vector> 

namespace Ath
{
  enum Type {
    kNone,
    kFloat,
    kDouble,
    kInt,
    kUInt,
    kBool,
    kShort,
    kLong,
    kULong,
    kVecFloat,
    kVecInt
  };

  std::string GetTypeAsStr  (Type               type);
  Type        GetTypeFromStr(const std::string &type);

  class VarEntry
  {
  public:
    
    VarEntry();
    VarEntry(unsigned key, double value, Type type=kNone);
    ~VarEntry() {}
    
    unsigned GetKey  () const { return fKey;  }
    Type     GetType () const { return fType; }

    double   GetVal  () const { return fData; }
    double   GetData () const { return fData; }
    double   GetValue() const { return fData; }
    
    void Print(std::ostream &os = std::cout) const;
  
  private:
    
    uint32_t fKey;     // variable key
    double   fData;    // variable value
    Type     fType;
  };
 
  //
  // Vector for non-permanent variables
  //
  typedef std::vector<Ath::VarEntry> VarEntryVec;

  //
  // Inlined functions
  //
  inline VarEntry::VarEntry()
    :fKey(0), fData(0.), fType(kNone)
  {
  }

  inline VarEntry::VarEntry(const unsigned int key, double data, Type type)
    :fKey(key), fData(data), fType(type)
  {
  }
 
  //
  // Inlined comparison operators
  //
  inline bool operator==(const VarEntry &lhs, const VarEntry &rhs) { 
    return lhs.GetKey() == rhs.GetKey();
  }
  inline bool operator <(const VarEntry &lhs, const VarEntry &rhs) { 
    return lhs.GetKey() < rhs.GetKey();
  }

  inline bool operator==(const VarEntry &var, unsigned key) { return var.GetKey() == key; }
  inline bool operator==(unsigned key, const VarEntry &var) { return var.GetKey() == key; }

  inline bool operator<(const VarEntry &var, unsigned key) { return var.GetKey() < key; }
  inline bool operator<(unsigned key, const VarEntry &var) { return key < var.GetKey(); }
}

#endif
