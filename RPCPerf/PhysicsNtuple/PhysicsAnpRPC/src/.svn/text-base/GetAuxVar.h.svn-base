// This is -*- c++ -*-
#ifndef ATH_GETAUXVAR_H
#define ATH_GETAUXVAR_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : GetAuxVar
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 *  
 * Read auxiliary variables from xAOD objects into Ath::Object
 *
 **********************************************************************************/

// C/C++
#include <stdint.h>
#include <iostream>
#include <vector> 

// xAOD
#include "xAODBase/IParticle.h"

// Local
#include "VarDefs.h"
#include "VarHolder.h"
#include "WriteUtils.h"

namespace Ath
{
  //=============================================================================
  class AuxVarData
  {
  public:
    
    AuxVarData();
    AuxVarData(const VarConfig &conf, bool debug);
    ~AuxVarData() {}
    
    uint32_t GetVar ()         const { return fVarConfig.var;           }
    bool     IsValid()         const { return fIsValid;                 }
    double   GetData()         const { return fData;                    }
    double   GetDefaultValue() const { return fVarConfig.default_value; }
    
    Type               GetType() const { return fVarConfig.type; }
    const std::string& GetName() const { return fVarConfig.name; }

    void SetDebug(bool flag) { fDebug = flag; }

    bool ReadAuxData(const xAOD::IParticle &particle);

    void Print(std::ostream &os = std::cout) const;
  
  private:

    bool InitAuxAccess(const xAOD::IParticle &particle);
    bool PrepAuxAccess(const std::string &auxtype);

  private:

    typedef SG::AuxElement::ConstAccessor<float>         AccessFloat;
    typedef SG::AuxElement::ConstAccessor<short>         AccessShort;
    typedef SG::AuxElement::ConstAccessor<int>           AccessInt;
    typedef SG::AuxElement::ConstAccessor<unsigned int>  AccessUInt;
    typedef SG::AuxElement::ConstAccessor<char>          AccessChar;
    typedef SG::AuxElement::ConstAccessor<unsigned char> AccessUChar;

  private:
    
    VarConfig    fVarConfig; // variable configuration
    double       fData;      // variable value
    
    bool         fIsInit;
    bool         fIsValid;
    bool         fDebug;

    AccessFloat *fAccFloat;
    AccessShort *fAccShort;
    AccessInt   *fAccInt;
    AccessUInt  *fAccUInt;
    AccessChar  *fAccChar;
    AccessUChar *fAccUChar;
  };
  
  //=============================================================================
  class GetAuxList
  {
  public:
    
    GetAuxList() :fDebug(false) {}
    ~GetAuxList();
    
    bool BookAuxList(const std::vector<std::string> &vars);

    bool ReadAuxList(const xAOD::IParticle &particle);

    bool FillVarList(VarHolder &vars);

    void SetDebug(bool flag) { fDebug = flag; }

    void Print(std::ostream &os = std::cout) const;
  
  private:

    struct Mesg
    {
      explicit Mesg(const std::string &m) :count(0), msg(m), var(Def::NONE) {}

      unsigned    count;
      std::string msg;
      uint32_t    var;
    };

    typedef std::map<uint32_t, Mesg> MesgMap;
    typedef std::vector<AuxVarData> VarVec;

  private:

    bool PrintMesg(uint32_t var, const std::string &mesg);

  private:
    
    VarVec   fVars;
    bool     fDebug;
    MesgMap  fMesgMap;
  };

  //=============================================================================
  // Inlined comparison operators
  //
  inline bool operator==(const AuxVarData &lhs, const AuxVarData &rhs) { 
    return lhs.GetVar() == rhs.GetVar();
  }
  inline bool operator <(const AuxVarData &lhs, const AuxVarData &rhs) { 
    return lhs.GetVar() < rhs.GetVar();
  }

  inline bool operator==(const AuxVarData &var, unsigned key) { return var.GetVar() == key; }
  inline bool operator==(unsigned key, const AuxVarData &var) { return var.GetVar() == key; }

  inline bool operator<(const AuxVarData &var, unsigned key) { return var.GetVar() < key; }
  inline bool operator<(unsigned key, const AuxVarData &var) { return key < var.GetVar(); }
}

#endif
