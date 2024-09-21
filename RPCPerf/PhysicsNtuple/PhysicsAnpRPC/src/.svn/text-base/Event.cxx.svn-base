
// C/C++
#include <iomanip>

// Local
#include "Event.h"
  
using namespace std;

//=============================================================================
Ath::Event::Event():
  fIsSimulation  (false),
  fHasPriVertex  (false),
  fPRWHash       (0),
  fPRWRun        (0),
  fPRWYear       (0),
  fRunNumber     (0),
  fEventNumber   (0),
  fMCChannel     (0.0),
  fMCWeight      (0.0),
  fPriVtxZ       (0.0),
  fBeamPosSigmaX (0.0),
  fBeamPosSigmaY (0.0),
  fBeamPosSigmaXY(0.0)
{
}

//=============================================================================
Ath::Event Ath::Event::MakeCopy() const
{
  //
  // Make deep copy of self - EXTREMELY IMPORTANT TO GET THIS RIGHT!!!
  //
  Event copy;

  return copy;
}

//=============================================================================
bool Ath::Event::AddVec(const std::string &key, const std::vector<Object> &vec)
{
  const bool good = fObjMap.insert(ObjectMap::value_type(key, vec)).second;
  
  if(!good) {
    cout << "Event::AddVec - duplicate key: " << key << endl;
  }

  return good;
}

//=============================================================================
std::vector<Ath::Object>& Ath::Event::GetVec(const std::string &key)
{
  return fObjMap[key];
}

//=============================================================================
void Ath::Event::ClearEvent() 
{
  //
  // Release all smart pointers - EXTREMELY IMPORTANT TO GET THIS RIGHT!!!
  //
  fObjMap.clear();

  GetVars().clear();
}

//=============================================================================
void Ath::Event::Print(std::ostream &os, unsigned nspace) const
{
  //
  // Print event
  //
  const string s(nspace, ' ');

  os << s << "Event::Print " << endl
     << s << "   Number of vars: " << GetVars().size() << endl;
  
  for(const VarEntry &var: GetVars()) {
    os << s << "   key=" << setw(10) << var.GetKey() << " var=" << var.GetData() << endl;
  }

  os << s << "   Number of object keys: " << fObjMap.size() << endl;

  for(const ObjectMap::value_type &v: fObjMap) {
    os << s << "   key==" << v.first << endl;

    for(const Object &o: v.second) {
      for(const VarEntry &var: o.GetVarEntry()) {
	os << s << "      key=" << setw(10) << var.GetKey() << " var=" << var.GetData() << endl;
      }
    }
  }
}
