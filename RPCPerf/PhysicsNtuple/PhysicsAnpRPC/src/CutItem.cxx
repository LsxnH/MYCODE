
// C/C++
#include <iomanip>
#include <sstream>

// ROOT
#include "TDirectory.h"

// Local
#include "Utils.h"
#include "CutItem.h"

using namespace std;

namespace Ath
{
  static unsigned cut_name_width = 0;
}

//==============================================================================
Ath::CutItem::CutItem():
  fDebug    (false),
  fDummy    (false)
{
}

//==============================================================================
bool Ath::CutItem::InitCut(const std::string &config)
{
  //
  // Read cut configuration
  //
  fCutName = config;
  fCutConf = config;

  fDebug  = false;
  fDummy  = false;

  //
  // Initialize comparison points
  //
  std::vector<string> configs;
  
  const bool isOR  = config.find("||") != string::npos;
  const bool isAND = config.find("&&") != string::npos;

  if(isOR && isAND) {
    log() << "InitCut - invalid configuration string: " << config << endl;
    return false;
  }
  
  if(isOR) { 
    Ath::StringTok(configs, config, "|"); 

    for(unsigned i = 0; i < configs.size(); ++i) {
      AddComparePoint(configs.at(i), true);
    }
  }
  else {
    Ath::StringTok(configs, config, "&"); 

    for(unsigned i = 0; i < configs.size(); ++i) {
      AddComparePoint(configs.at(i), false);
    }    
  }

  //
  // Save name width for pretty printing of many cuts
  //
  cut_name_width = std::max<unsigned>(cut_name_width, fCutName.size());

  return true;
}

//==============================================================================
void Ath::CutItem::PrintConfig(std::ostream &os, const std::string &pad) const
{  
  os << ConfigAsString(pad);
}

//==============================================================================
std::string Ath::CutItem::ConfigAsString(const string &pad) const
{
  //
  // Print config
  //
  stringstream os;

  const string name = Ath::PadStrBack(GetName(), cut_name_width);

  if(!fCompA.empty()) {
    if(fCompA.size() == 1) {
      os << pad << "CutItem - " << name << ": print 1 condition:" << endl;
    }
    else {
      os << pad << "CutItem - " << name << ": print " << fCompA.size() << " AND condition(s):" << endl;      
    }

    for(const ComparePoint &p: fCompA) {
      os << pad << "          " << setw(20) << std::left << p.var << " "
	 << std::setw(2) << std::left << GetString(p.compare) << " " << p.cut;

      if(p.use_abs) {
	os << "  --> use abs" << endl;
      }
      else {
	os << endl;
      }
    }
  }

  if(!fCompO.empty()) {
    os << pad << "CutItem - " << name << ": print " << fCompO.size() << " OR  condition(s):" << endl;

    for(const ComparePoint &p: fCompO) {
      os << pad << "          " << setw(20) << std::left << p.var << " "
	 << std::setw(2) << std::left << GetString(p.compare) << " " << p.cut;

      if(p.use_abs) {
	os << "  --> use abs" << endl;
      }
      else {
	os << endl;
      }
    }
  } 

  return os.str();
}

//==============================================================================
bool Ath::CutItem::AddComparePoint(const std::string &config, bool isOR)
{
  //
  // Add comparison point if exists
  //
  ComparePoint p;
  
  for(unsigned i = 0; i < GetAllComp().size(); ++i) {
    p.compare = GetAllComp().at(i);
    p.opr     = GetString(p.compare);

    //
    // Try to extract variable name and cut value
    //
    const size_t ipos = config.find(p.opr);
    
    if(ipos == string::npos) {
      if(fDebug) {
	log() << "AddComparePoint - " << p.opr << ": failed to parse config: " << config << endl;
      }
      continue;
    }

    //
    // Remove white spaces
    //
    const string part1 = StripSpaces(config.substr(0, ipos));
    const string part2 = StripSpaces(config.substr(ipos+p.opr.size()));

    //
    // Try to match variable name to enum
    //
    p.var = part1;
    p.key     = ParseKey(p.var).first;
    p.use_abs = ParseKey(p.var).second; 
  
    
    if(p.key == 0) {
      log() << "AddComparePoint - failed to match var: " << config << endl;
      return false;
    }
    
    //
    // Extact cut value
    //
    stringstream str;
    
    str << part2;
    str >> p.cut;
    
    if(str.fail()) {
      log() << "AddComparePoint - failed to extract value: " << config << endl;
      return false;
    }

    break;
  }

  if(p.key == 0) {
    log() << "AddComparePoint - failed to extract operator: " << config << endl;
    return false;
  }

  if(fDebug) {
    log() << "AddComparePoint - new comparison point: \"" << config << "\"" << endl;
  }

  if(isOR) {
    fCompO.push_back(p);
  }
  else {
    fCompA.push_back(p);
  }

  return true;
}

//==============================================================================
std::string Ath::CutItem::GetString(Compare comp, bool print_nice)
{
  if(print_nice) {

    switch(comp)
      {
      case Less:           return "<";
      case LessOrEqual:    return "<=";
      case Greater:        return ">";
      case GreaterOrEqual: return ">=";
      case Equal:          return "==";
      case NotEqual:       return "!=";
      default  :           break;
      }
  }
  else {
    switch(comp)
      {
      case Less:           return "&lt;";
      case LessOrEqual:    return "&lt;=";
      case Greater:        return "&gt;";
      case GreaterOrEqual: return "&gt;=";
      case Equal:          return "&eq;";
      case NotEqual:       return "!&eq;";
      default  :           break;
      }
  }

  return "None";
}

//==============================================================================
Ath::CutItem::Compare Ath::CutItem::GetCompare(const std::string &val) const
{
  //
  // Get comparison operator from string
  //
  if     (val == GetString(Less))           return Less;
  else if(val == GetString(LessOrEqual))    return LessOrEqual;
  else if(val == GetString(Greater))        return Greater;
  else if(val == GetString(GreaterOrEqual)) return GreaterOrEqual;
  else if(val == GetString(Equal))          return Equal;
  else if(val == GetString(NotEqual))       return NotEqual;

  return None;
}

//==============================================================================
std::ostream& Ath::CutItem::log() const
{
  std::cout << GetName() << "::";
  return std::cout;
}

//==============================================================================
const std::vector<Ath::CutItem::Compare>& Ath::CutItem::GetAllComp()
{
  static vector<Compare> cvec;

  if(cvec.empty()) {
    //
    // Fill vector of all operators - order matters!
    //
    cvec.push_back(LessOrEqual);
    cvec.push_back(Less);
    cvec.push_back(GreaterOrEqual);
    cvec.push_back(Greater);
    cvec.push_back(Equal);
    cvec.push_back(NotEqual);
  }

  return cvec;
}

//==============================================================================
std::string Ath::CutItem::StripSpaces(const std::string &s) const
{
  //
  // Strip leading and trailing whitespaces
  //
  if(s.empty()) {
    log() << "StripSpaces - empty string" << endl;
    return s;
  }

  //
  // Split string in separate parts separated by whitespace
  //
  std::vector<string> parts;
  Ath::StringTok(parts, s, " ");

  if(parts.size() != 1) {
    log() << "StripSpaces - invalid input string: \"" << s << "\"" << endl;
    return s;    
  }

  return parts.front();
}

//==============================================================================
std::pair<uint32_t, bool> Ath::CutItem::ParseKey(const std::string &key) const
{
  std::pair<uint32_t, bool> result(Def::NONE, false);
    
  const size_t open  = key.find("[");
  const size_t close = key.find("]");

  if(open == string::npos && close == string::npos) {
    result.first = Def::Convert2Var(key);
  }
  else if(open != string::npos && close != string::npos) {
    if(!(open < close)) {
      throw std::runtime_error("CutItem::ParseKey - invalid key brackets: " + key);
    }
    
    const string s1 = key.substr(0, open);
    const string s2 = key.substr(open+1, close-open-1);

    if(fDebug) {
      cout << "key = " << key << endl
	   << "s1  = " << s1  << endl
	   << "s2  = " << s2  << endl
	   << "open  = " << open  << endl
	   << "close = " << close << endl;
    }

    if(s1 != "Abs") {
      throw std::runtime_error("CutItem::ParseKey - invalid operator (only Abs works for now): " + key);      
    }

    result.first  = Def::Convert2Var(s2);
    result.second = true;
  }
  else {
    throw std::runtime_error("CutItem::ParseKey - invalid key: " + key);
  }

  return result;
}
