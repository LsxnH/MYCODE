
// C/C++
#include <cstdlib>
#include <climits>
#include <iomanip>

// ROOT
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

// Local
#include "VarEntry.h"

using namespace std;

//=============================================================================
std::string Ath::GetTypeAsStr(Type type)
{
  if     (type == kFloat)    return "Float";
  else if(type == kDouble)   return "Double";
  else if(type == kInt)      return "Int";
  else if(type == kLong)     return "Long";
  else if(type == kUInt)     return "UInt";
  else if(type == kULong)    return "ULong";
  else if(type == kBool)     return "Bool";
  else if(type == kShort)    return "Short";
  else if(type == kVecFloat) return "VecFloat";
  else if(type == kVecInt)   return "VecInt";

  return "None";
}

//=============================================================================
Ath::Type Ath::GetTypeFromStr(const string &type)
{
  if     (type == "Float")     return kFloat;
  else if(type == "Double")    return kDouble;
  else if(type == "Int")       return kInt;
  else if(type == "Long")      return kLong;
  else if(type == "UInt")      return kUInt;
  else if(type == "ULong")     return kULong;
  else if(type == "Bool")      return kBool;
  else if(type == "Short")     return kShort;
  else if(type == "VecInt")    return kVecInt;
  else if(type == "VecFloat")  return kVecFloat;

  return kNone;
}

//=============================================================================
void Ath::VarEntry::Print(std::ostream &os) const
{
  os << "VarEntry::Print" << std::endl;
}
