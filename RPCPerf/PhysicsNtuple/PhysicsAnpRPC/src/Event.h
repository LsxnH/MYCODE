// -*- c++ -*-
#ifndef ATH_EVENT_H
#define ATH_EVENT_H

/**********************************************************************************
 * @Package: PhysicsAnpRPC
 * @Class  : Event
 * @Author : Rustem Ospanov
 *
 * @Brief  :
 * 
 *  Event is a data class which holds data and MC objects to save
 * 
 **********************************************************************************/

// C++
#include <map>
#include <vector>

// ROOT
#include "Rtypes.h"

// Data
#include "Object.h"

namespace Ath
{
  //
  // Global typedefs
  //
  typedef std::vector<Object> ObjectVec;

  class Event: public virtual VarHolder
  {
  public:
    
    Event();
    virtual ~Event() {}
    
    std::string GetObjectType() const { return "Event"; }

    void SetSimulationFlag(bool      val) { fIsSimulation   = val; }
    void SetPriVertexFlag (bool      val) { fHasPriVertex   = val; }
    void SetPRWHash       (ULong64_t val) { fPRWHash        = val; }
    void SetPRWRun        (int       val) { fPRWRun         = val; }
    void SetPRWYear       (int       val) { fPRWYear        = val; }

    void SetRunNumber     (int       val) { fRunNumber      = val; }
    void SetEventNumber   (int64_t   val) { fEventNumber    = val; }
    void SetMCChannel     (int       val) { fMCChannel      = val; }
    void SetMCWeight      (double    val) { fMCWeight       = val; }
    void SetPriVtxZ       (double    val) { fPriVtxZ        = val; }
    void SetBeamPosSigmaX (double    val) { fBeamPosSigmaX  = val; }
    void SetBeamPosSigmaY (double    val) { fBeamPosSigmaY  = val; }
    void SetBeamPosSigmaXY(double    val) { fBeamPosSigmaXY = val; }

    bool      IsSimulation     () const { return fIsSimulation;   }
    bool      HasPriVertex     () const { return fHasPriVertex;   }

    ULong64_t GetPRWHash       () const { return fPRWHash;        }
    int       GetPRWRun        () const { return fPRWRun;         }
    int       GetPRWYear       () const { return fPRWYear;        }
    int       GetRunNumber     () const { return fRunNumber;      }
    int64_t   GetEventNumber   () const { return fEventNumber;    }
    int       GetMCChannel     () const { return fMCChannel;      }

    double    GetMCWeight      () const { return fMCWeight;       }
    double    GetPriVtxZ       () const { return fPriVtxZ;        }
    double    GetBeamPosSigmaX () const { return fBeamPosSigmaX;  }
    double    GetBeamPosSigmaY () const { return fBeamPosSigmaY;  }
    double    GetBeamPosSigmaXY() const { return fBeamPosSigmaXY; }

    Event MakeCopy() const;   

    bool AddVec(const std::string &key, const std::vector<Object> &vec);

    std::vector<Object>& GetVec(const std::string &key);

    void ClearEvent();
    
    void Print(std::ostream &os = std::cout, unsigned nspace = 0) const;
    
  private:

    typedef std::map<std::string, ObjectVec> ObjectMap;

  private:

    bool      fIsSimulation;
    bool      fHasPriVertex;
    ULong64_t fPRWHash;
    int       fPRWRun;
    int       fPRWYear;

    int       fRunNumber;
    int64_t   fEventNumber;
    int       fMCChannel;

    double    fMCWeight;
    double    fPriVtxZ;
    double    fBeamPosSigmaX;
    double    fBeamPosSigmaY;
    double    fBeamPosSigmaXY;

    ObjectMap fObjMap;
  };
}

#include "SGTools/CLASS_DEF.h"
CLASS_DEF( Ath::Event , 94097965 , 1 )

#endif
