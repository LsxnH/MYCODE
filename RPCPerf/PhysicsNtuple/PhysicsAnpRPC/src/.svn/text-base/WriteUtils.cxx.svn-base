
// C/C++
#include <cstdlib>
#include <climits>
#include <iomanip>
#include <sstream>

// ROOT
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

// Local
#include "Event.h"
#include "Utils.h"
#include "WriteUtils.h"

using namespace std;

//=============================================================================
bool Ath::VarConfig::ConfigVarFromStr(const std::string &config)
{
  //
  // Configure variable from string of type: "name:type=value:nick=value:default=value"
  //
  conf = RemoveWhiteSpaces(config);

  vector<string> parts;
  Ath::StringTok(parts, conf, ":");
  
  if(debug) {
    cout << "ConfigVarFromStr - conf=" << conf << endl;
  
    for(unsigned i = 0; i < parts.size(); ++i) {
      cout << "   part[" << setw(2) << i << "] = \"" << parts[i] << "\"" << endl;
    }
  }

  if(parts.empty()) {
    return false;
  }
  
  name = parts.at(0);
  nick = name; 
  
  for(unsigned i = 1; i < parts.size(); ++i) {
    const string &part = parts.at(i);

    string variable, value;

    if(!ReadValueFromStr(part, variable, value)) {
      continue;
    }

    if(variable == "type") { 
      type = Ath::GetTypeFromStr(value);
    }
    if(variable == "nick") { 
      nick = value;
    }
    if(variable == "default") { 
      default_value = std::stod(value);
    }
  }

  if(Def::IsKnownKey(nick)) {
    var = Def::Convert2Var(nick);
  }
  else {
    var = Def::RegisterDynamicVar(nick);
  }
  
  return true;
}

//=============================================================================
bool Ath::VarConfig::ReadValueFromStr(const std::string &input, std::string &variable, std::string &value)
{
  //
  // Decompose "part", assuming it has form: "variable=value"
  //
  if(input.find("=") == string::npos) {
    return false;
  }

  vector<string> parts;
  Ath::StringTok(parts, input, "=");

  if(parts.size() != 2) {
    cout << "VarConfig::ReadValueFromStr - failed to parse input=\"" << input << "\"" << endl;
    return false;
  }

  variable = parts.at(0);
  value    = parts.at(1);

  return true;
}

//=============================================================================
std::string Ath::VarConfig::RemoveWhiteSpaces(std::string str)
{
  std::string::iterator sit = str.begin();
  
  while(sit != str.end()) {
    if((*sit) == ' ') {
      sit = str.erase(sit);
    }
    else {
      sit++;
    }
  }

  return str;
}

//=============================================================================
std::string Ath::VarConfig::PrintVarConfig() const
{
  std::stringstream str;

  str << "VarConfig::PrintVarConfig - conf=\"" << conf << "\"" << endl
      << "   var:  \"" << var                << "\"" << endl
      << "   type: \"" << GetTypeAsStr(type) << "\"" << endl
      << "   name: \"" << name               << "\"" << endl
      << "   nick: \"" << nick               << "\"" << endl;

  return str.str();
}

//=============================================================================
Ath::Data::Data():
  val_flt(0.0), 
  val_dbl(0.0), 
  val_int(0), 
  val_snt(0), 
  val_lnt(0), 
  val_unt(0), 
  val_uln(0), 
  val_bln(0), 
  vec_flt(0), 
  vec_dbl(0), 
  vec_int(0), 
  vec_snt(0), 
  vec_lnt(0), 
  vec_unt(0), 
  vec_uln(0), 
  vec_bln(0), 
  vc2_flt(0),
  vc2_int(0),
  var    (Def::NONE),
  type   (kNone)
{
}

//=============================================================================
bool Ath::Data::InitFromVarEntry(const VarEntry &entry)
{
  //
  // Initilize self from var entry
  //

  if(entry.GetType() == kNone) {
    return false; // missing variable type
  }
  
  var  = entry.GetKey();
  type = entry.GetType();
  
  if(var == Def::NONE) {
    std::cout << "InitFromVarEntry - unknown variable: " << entry.GetKey() << std::endl;
    return false; 
  }

  name = Def::Convert2Str(entry.GetKey());
  nick = name;

  return true;
}

//=============================================================================
bool Ath::Data::InitFromVarConfig(const VarConfig &config)
{
  //
  // Initilize self from var entry
  //

  if(config.type == kNone || config.var == Def::NONE) {
    throw std::logic_error("Data::InitFromVarConfig - invalid VarConfig: " + config.name);
  }
  
  var  = config.var;
  type = config.type;
  name = config.name;
  nick = config.nick;

  return true;
}

//=============================================================================
TBranch* Ath::Data::BookVarBranch(TTree *tree)
{
  //
  // Create output tree
  //
  TBranch *branch = 0;

  if(!tree) {
    return branch;
  }
  
  if     (type == kFloat)  { branch = tree->Branch(nick.c_str(), &val_flt, (nick+"/F").c_str()); }
  else if(type == kDouble) { branch = tree->Branch(nick.c_str(), &val_dbl, (nick+"/D").c_str()); }
  else if(type == kInt)    { branch = tree->Branch(nick.c_str(), &val_int, (nick+"/I").c_str()); }
  else if(type == kShort)  { branch = tree->Branch(nick.c_str(), &val_snt, (nick+"/S").c_str()); }
  else if(type == kLong)   { branch = tree->Branch(nick.c_str(), &val_lnt, (nick+"/L").c_str()); }
  else if(type == kUInt)   { branch = tree->Branch(nick.c_str(), &val_unt, (nick+"/i").c_str()); }
  else if(type == kULong)  { branch = tree->Branch(nick.c_str(), &val_uln, (nick+"/l").c_str()); }
  else if(type == kBool)   { branch = tree->Branch(nick.c_str(), &val_bln, (nick+"/O").c_str()); }
  else {
    cout << "Data::BookVarBranch - ERROR: invalid variable type: " << type << endl;
  }

  return branch;
}

//=============================================================================
TBranch* Ath::Data::BookVecBranch(TTree *tree, const string &prefix)
{
  //
  // Create output tree
  //
  TBranch *branch = 0;

  if(!tree) {
    return branch;
  }

  if(type == kFloat) {
    vec_flt = new std::vector<Float_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_flt);
  }
  else if(type == kDouble) {
    vec_dbl = new std::vector<Double_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_dbl);
  }
  else if(type == kInt) {
    vec_int = new std::vector<Int_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_int);
  }
  else if(type == kShort) {
    vec_snt = new std::vector<Short_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_snt);
  }
  else if(type == kLong) {
    vec_lnt = new std::vector<Long64_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_lnt);
  }
  else if(type == kUInt) {
    vec_unt = new std::vector<UInt_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_unt);
  }
  else if(type == kULong) {
    vec_uln = new std::vector<uint64_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_uln);
  }
  else if(type == kBool) {
    vec_bln = new std::vector<Bool_t>();
    branch  = tree->Branch((prefix+nick).c_str(), &vec_bln);
  }
  else if(type == kVecFloat) {
    vc2_flt = new std::vector<std::vector<Float_t> >();
    branch  = tree->Branch((prefix+nick).c_str(), &vc2_flt);
  }
  else if(type == kVecInt) {
    vc2_int = new std::vector<std::vector<Int_t> >();
    branch  = tree->Branch((prefix+nick).c_str(), &vc2_int);
  }
  else {
    cout << "Data::BookVecBranch - ERROR: invalid variable type: " << type << endl;
  }

  return branch;
}

//=============================================================================
Ath::Type Ath::GetDataType(string type)
{
  if(type.find("type=") != string::npos) {
    vector<string> parts;
    Ath::StringTok(parts, type, "=");
    
    if(parts.size() == 2) {
      type = parts.at(1);
    }
  }

  return GetTypeFromStr(type);
}

//=============================================================================
std::string Ath::GetDataNick(string nick)
{
  if(nick.find("nick=") != string::npos) {
    vector<string> parts;
    Ath::StringTok(parts, nick, "=");
    
    if(parts.size() == 2) {
      return parts.at(1);
    }
  }

  return "";
}

//=============================================================================
bool Ath::HasKeyInDataVec(const DataVec &dvec, unsigned key)
{
  for(const Data &data: dvec) {
    if(data.var == key) {
      return true;
    }
  }
  
  return false;
}
