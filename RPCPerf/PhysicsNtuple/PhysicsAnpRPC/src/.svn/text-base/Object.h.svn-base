// -*- c++ -*-
#ifndef ATH_OBJECT_H
#define ATH_OBJECT_H

/**********************************************************************************
 * @Package: PhysicsAnpData
 * @Class  : ObjectBase
 * @Author : Rustem Ospanov
 *
 * @Brief  : Class for storing object data
 * 
 **********************************************************************************/

// C/C++
#include <string>

// xAOD
#include "xAODBase/IParticle.h"

// Local
#include "VarHolder.h"

namespace Ath
{
  class Object : public VarHolder
  {    
  public:
    
    Object();
    Object(xAOD::IParticle *p);    
    virtual ~Object() {}
    
    void SetParticle(xAOD::IParticle *p) { fParticle = p; }
    
    xAOD::IParticle *GetParticle() const { return fParticle; }

    static std::string GetObjectType() { return "Object"; }

  private:
    
    xAOD::IParticle *fParticle;
  };

  //-------------------------------------------------------------------------------
  // Inlined functions
  //
  inline Object::Object():
    fParticle(0)
  {
  }

  inline Object::Object(xAOD::IParticle *p):
    fParticle(p)
  {
  }
}

#endif
