// Local
#include "GetAuxVar.h"

using namespace std;

//=============================================================================
Ath::AuxVarData::AuxVarData():
  fVarConfig(),
  fData     (0.0),
  fIsInit   (false),
  fIsValid  (false),
  fDebug    (false),
  fAccFloat (0),
  fAccShort (0),
  fAccInt   (0),
  fAccUInt  (0),
  fAccChar  (0),
  fAccUChar (0)
{
}

//=============================================================================  
Ath::AuxVarData::AuxVarData(const VarConfig &config, bool debug):
  fVarConfig(config), 
  fData     (0.0),
  fIsInit   (false),
  fIsValid  (false),
  fDebug    (debug),
  fAccFloat (0),
  fAccShort (0),
  fAccInt   (0),
  fAccUInt  (0),
  fAccChar  (0),
  fAccUChar (0)
{
}

//=============================================================================
void Ath::AuxVarData::Print(std::ostream &os) const
{
  os << "VarEntry::Print" << std::endl;
}

//=============================================================================
bool Ath::AuxVarData::ReadAuxData(const xAOD::IParticle &particle)
{
  if(fDebug) {
    cout << "AuxVarData::ReadAuxData - begin" << endl
	 << fVarConfig.PrintVarConfig() << endl;
  }

  if(!fIsInit) {
    InitAuxAccess(particle);
  }

  if(!fIsValid) {
    return false;
  }

  if     (fAccFloat) { fData = (*fAccFloat)(particle); }
  else if(fAccShort) { fData = (*fAccShort)(particle); }
  else if(fAccInt)   { fData = (*fAccInt)  (particle); }
  else if(fAccUInt)  { fData = (*fAccUInt) (particle); }
  else if(fAccChar)  { fData = (*fAccChar) (particle); }
  else if(fAccUChar) { fData = (*fAccUChar)(particle); }
  else {
    return false;
  }

  if(fDebug) {
    cout << "AuxVarData::ReadAuxData - value=" << fData << endl;
  }

  return true;
}

//=============================================================================
bool Ath::AuxVarData::InitAuxAccess(const xAOD::IParticle &particle)
{
  if(fIsInit) {
    return true;
  }

  fIsInit  = true;
  fIsValid = false;

  static SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
  
  for(SG::auxid_t auxid: particle.getAuxIDs()) {
    const string aname = reg.getName(auxid);
    const string atype = reg.getTypeName(auxid);
    
    if(aname == fVarConfig.name) {
      fIsValid = PrepAuxAccess(atype);
      cout << "AuxVarData::InitAuxAccess - valid=" << fIsValid << " var: \"" << fVarConfig.name << "\"" << endl;
      break;
    }
  }

  return fIsValid;
}

//=============================================================================
bool Ath::AuxVarData::PrepAuxAccess(const std::string &auxtype)
{
  if(fVarConfig.var == Def::NONE) {
    return false;
  }

  bool result = true;

  if     (auxtype == "float")         { fAccFloat = new AccessFloat(fVarConfig.name); }
  else if(auxtype == "short")         { fAccShort = new AccessShort(fVarConfig.name); }
  else if(auxtype == "int")           { fAccInt   = new AccessInt  (fVarConfig.name); }
  else if(auxtype == "unsigned int")  { fAccUInt  = new AccessUInt (fVarConfig.name); }
  else if(auxtype == "char")          { fAccChar  = new AccessChar (fVarConfig.name); }
  else if(auxtype == "unsigned char") { fAccUChar = new AccessUChar(fVarConfig.name); }
  else {
    cout << "AuxVarData::PrepAuxAccess - uknown type: \"" << auxtype << "\"" << endl;
    result = false;
  }

  return result;
}

//=============================================================================  
Ath::GetAuxList::~GetAuxList()
{
  if(!fMesgMap.empty()) {
    std::cout << "GetAuxList - print " << fMesgMap.size() << " messages" << std::endl;  
    
    for(const MesgMap::value_type &m: fMesgMap) {
      std::cout << "   " << m.second.msg << Def::AsStr(m.first) << std::endl;
    }
  }
}

//=============================================================================
bool Ath::GetAuxList::BookAuxList(const std::vector<std::string> &configs)
{
  if(fDebug) {
    cout << "--------------------------------------------------------------------------------" << endl
	 << "GetAuxList::BookAuxList - process " << configs.size() << " string config objects" << endl;
  }
  
  for(const std::string &config: configs) {
    VarConfig vc;
    
    if(!vc.ConfigVarFromStr(config)) {
      throw std::logic_error("GetAuxList::BookAuxList - failed to configure variable: \"" + config + "\"");
    }

    if(fDebug) {
      cout << vc.PrintVarConfig() << endl;
    }

    fVars.push_back(Ath::AuxVarData(vc, fDebug));
  }
  
  return true;
}

//=============================================================================
bool Ath::GetAuxList::ReadAuxList(const xAOD::IParticle &particle)
{
  unsigned icount = 0;

  for(AuxVarData &var: fVars) {
    if(var.ReadAuxData(particle)) {
      icount++;
    } 
    else {
      PrintMesg(var.GetVar(), "AuxVarData::ReadAuxList - missing variable: " + var.GetName()); 
    }
  }
  
  return (icount == fVars.size());
}

//=============================================================================
bool Ath::GetAuxList::FillVarList(VarHolder &vars)
{
  unsigned icount = 0;
  
  for(const AuxVarData &var: fVars) {
    double data = var.GetDefaultValue();

    if(var.IsValid()) {
      data = var.GetData();
    }

    if(vars.AddVar(var.GetVar(), data, var.GetType())) {
      icount++;
      
      if(fDebug) {
	cout << "GetAuxList::FillVarList - added var=" << var.GetName() << " data=" << var.GetData() << endl;
      }
      
    }
    else {
      std::cout << "GetAuxList::FillVarList - failed to add var=" << var.GetName() << std::endl;
    }
  }

  return (icount == fVars.size());
}

//=============================================================================
bool Ath::GetAuxList::PrintMesg(uint32_t var, const std::string &mesg)
{
  Mesg &m = fMesgMap.insert(MesgMap::value_type(var, Mesg(mesg))).first->second;
  
  if(m.count++ <= 1) {
    std::cout << mesg << std::endl;
    return true;
  }
  
  return false;
}
