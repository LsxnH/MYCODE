
// C/C++
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <sys/stat.h>
#include <stdint.h>
#include <stdexcept>

// Athena
#include "GaudiKernel/AlgTool.h"

// xAOD
#include "xAODBase/IParticle.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

// Local
#include "Event.h"
#include "VarDefs.h"
#include "Utils.h"

using namespace std;

//=============================================================================
void Ath::StringTok(std::vector<std::string>& ls,
		    const std::string& str,
		    const std::string& tok)
{
  //======================================================================
  // Split a long string into a set of shorter strings spliting along
  // divisions makers by the characters listed in the token string
  //======================================================================
  const string::size_type S = str.size();
  string::size_type  i = 0;  

  while (i < S) {
    // eat leading whitespace
    while (i < S && tok.find(str[i]) != string::npos) {
      ++i;
    }
    if (i == S) break;  // nothing left but WS
    
    // find end of word
    string::size_type  j = i+1;
    while (j < S && tok.find(str[j]) == string::npos) {
      ++j;
    }
    
    // add word
    ls.push_back(str.substr(i,j-i));
    
    // set up for next loop
    i = j+1;
  }  
}

//======================================================================================================
std::string Ath::PadStrFront(std::string str, int width)
{
  // Pad str with blanks
  if(width < 1) return str; 

  const int osize = str.size();
  for(int i = 0; i < width; ++i) {
    if(i >= osize) str.insert(str.begin(), ' ');
  }
  
  return str;
}

//======================================================================================================
std::string Ath::PadStrBack(std::string str, int width)
{
  // Pad str with blanks
  if(width < 1) return str; 

  const int osize = str.size();
  for(int i = 0; i < width; ++i) {
    if(i >= osize) str.push_back(' ');
  }
  
  return str;
}

//=============================================================================
bool Ath::AddMom(VarHolder &h, const xAOD::IParticle &p)
{
  h.AddVar(Ath::Def::Pt,     p.pt());
  h.AddVar(Ath::Def::Mass,   p.m());
  h.AddVar(Ath::Def::Energy, p.e());
  
  if(p.pt() > 1.0e-3) {
    h.AddVar(Ath::Def::Eta, p.eta());
    h.AddVar(Ath::Def::Phi, p.phi());
    
    return true;
  }

  h.AddVar(Ath::Def::Eta, -1.0e3);
  h.AddVar(Ath::Def::Phi, -1.0e3);

  return false;
}

//=============================================================================
bool Ath::AddImpactParameters(VarHolder &h, const xAOD::TrackParticle *track, const Event &event)
{
  if(!track) {
    return false;
  }

  const double z0 = track->z0() + track->vz() - event.GetPriVtxZ();
  
  h.AddVar(Ath::Def::D0,    track->d0());
  h.AddVar(Ath::Def::D0Sig, xAOD::TrackingHelpers::d0significance(track, 
								  event.GetBeamPosSigmaX(), 
								  event.GetBeamPosSigmaY(), 
								  event.GetBeamPosSigmaXY()));

  h.AddVar(Ath::Def::Z0,    z0);
  h.AddVar(Ath::Def::Z0Sin, z0*std::sin(track->theta()));
  h.AddVar(Ath::Def::Z0Sig, xAOD::TrackingHelpers::z0significance(track, NULL));

  return true;
}

//=============================================================================
std::string Ath::PrintResetStopWatch(TStopwatch &watch)
{
  watch.Stop();
  
  double realt = watch.RealTime();
  double cput  = watch.CpuTime();
  
  watch.Reset();
  watch.Start();
  
  const int hours = static_cast<int>(realt/3600.0);
  const int  min  = static_cast<int>(realt/60.0) - 60*hours;
  
  realt -= hours * 3600;
  realt -= min * 60;
  
  if (realt < 0) realt = 0;
  if (cput  < 0) cput  = 0;
  
  const int sec = static_cast<int>(realt);
  
  std::stringstream str;
  str << "Real time " 
      << setw(2) << setfill('0') << hours 
      << ":" << setw(2) << setfill('0') << min
      << ":" << setw(2) << setfill('0') << sec
      << " CPU time " << setprecision(3) << fixed << cput;
  
  return str.str();
}

//=============================================================================
namespace Ath
{
  typedef  std::map<const xAOD::TrackParticle *, uint32_t> TrackBarcodeMap;
  
  static TrackBarcodeMap gTrackBarcodes;
}

//=============================================================================
void Ath::resetTrackBarcodes()
{
  gTrackBarcodes.clear();
}

//=============================================================================
uint32_t Ath::getTrackBarcode(const xAOD::TrackParticle *track)
{
  return gTrackBarcodes.insert(TrackBarcodeMap::value_type(track, gTrackBarcodes.size()+1)).first->second;
}

//=============================================================================
std::string Ath::PrintAuxVars(const xAOD::IParticle &p, const std::string &prefix)
{
  static SG::AuxTypeRegistry& reg = SG::AuxTypeRegistry::instance();
  
  unsigned nsize = 0;
  unsigned tsize = 0;

  for(SG::auxid_t auxid : p.getAuxIDs()) {
    nsize = std::max<unsigned>(nsize, reg.getName    (auxid).size());
    tsize = std::max<unsigned>(tsize, reg.getTypeName(auxid).size());
  }

  std::stringstream str;

  for(SG::auxid_t auxid : p.getAuxIDs()) {
    const std::string n = "\"" + reg.getName    (auxid) + "\"";
    const std::string t = "\"" + reg.getTypeName(auxid) + "\"";

    str << prefix
	<< "AUX var name: " << std::setw(nsize+2) << std::left << n
	<< "  typename: "     << std::setw(tsize+2) << std::left << t
	<< std::endl;
  }

  return str.str();
}

//=============================================================================
int Ath::GetInt(const Ath::VarHolder &p, uint32_t var)
{
  double val = 0.0;

  if(!p.GetVar(var, val)) {
    std::cout << "GetInt - missing var: " << var << std::endl;
    return -1000;
  }
  
  return static_cast<int>(val);
}

//=============================================================================
std::string Ath::ParticleAsStr(const xAOD::TruthParticle &part)
{
  std::stringstream str;
 
  str << "PDG, status, barcode = ";
  
  const int pdg = part.pdgId();
  const int bar = part.barcode();
  const int sts = part.status();
  
  if(pdg >= 0) {
    str << " " << std::setw(3) << pdg << ", ";
  }
  else {
    str << "-" << std::setw(3) << std::abs(pdg) << ", ";
  }

  str << std::setw(2) << sts << ", "
      << std::setw(6) << bar << ", ";

  str << "  pt=" << std::setprecision(0) << std::fixed << std::setw(8) << part.pt();

  return str.str();
}

//=============================================================================
bool Ath::IsGeneratorParticle(const xAOD::TruthParticle &truth)
{
  const int status  = truth.status();
  const int barcode = truth.barcode();

  return barcode < 200000 && (status < 200 ||
			      status % 1000 == 1 || status % 1000 == 2 ||
			      status == 10902);
}

//=============================================================================
bool Ath::MatchPdgId(const xAOD::TruthParticle &truth, const std::vector<int> &pdgids)
{
  return std::find(pdgids.begin(), pdgids.end(), std::abs(truth.pdgId())) != pdgids.end();
}


//=============================================================================
void Ath::FillTruthParticle(Ath::Object &p, const xAOD::TruthParticle &part)
{
  p.AddVar(Ath::Def::PDG,            part.pdgId());
  p.AddVar(Ath::Def::Barcode,        part.barcode());
  p.AddVar(Ath::Def::TrueBarcode,    part.barcode());
  p.AddVar(Ath::Def::Status,         part.status());    
  
  Ath::AddMom(p, part);

  Ath::AddTruthParents (part, p);
  Ath::AddTruthChildren(part, p);
}

//=============================================================================
void Ath::FillTruthVertex(Ath::Object &v, const xAOD::TruthVertex &vtx)
{
  v.AddVar(Ath::Def::Barcode,     vtx.barcode());
  v.AddVar(Ath::Def::VtxBarcode,  vtx.barcode());
  v.AddVar(Ath::Def::NParents,    vtx.nIncomingParticles());      
  v.AddVar(Ath::Def::NChildren,   vtx.nOutgoingParticles());
  
  v.AddVar(Ath::Def::VtxX, vtx.x());
  v.AddVar(Ath::Def::VtxY, vtx.y());
  v.AddVar(Ath::Def::VtxZ, vtx.z());
  
  std::vector<int> particles_in, particles_out;
  
  for(const xAOD::TruthVertex::TPLink_t &vpit: vtx.incomingParticleLinks()) {
    if(!vpit.isValid()) {
      continue;
    }
    
    particles_in.push_back((*vpit)->barcode());
  }
  
  for(const xAOD::TruthVertex::TPLink_t &vpit: vtx.outgoingParticleLinks()) {
    if(!vpit.isValid()) {
      continue;
    }
    
    particles_out.push_back((*vpit)->barcode());
  }
  
  v.AddVec(Ath::Def::IndexRangeParent, particles_in);
  v.AddVec(Ath::Def::IndexRangeChild,  particles_out);
}

//=============================================================================
void Ath::AddTruthParents(const xAOD::TruthParticle &part, Ath::Object &p)
{
  std::vector<int> particles_in;

  if(part.prodVtx()) {
    p.AddVar(Ath::Def::ProdVtxBarcode, part.prodVtx()->barcode());

    for(const xAOD::TruthVertex::TPLink_t &vpit: part.prodVtx()->incomingParticleLinks()) {
      if(vpit.isValid()) {
	particles_in.push_back((*vpit)->barcode());
      }
    }
  }
  else {
    p.AddVar(Ath::Def::ProdVtxBarcode, 1.0e6);
  }

  p.AddVec(Ath::Def::IndexRangeParent, particles_in);  
}

//=============================================================================
void Ath::AddTruthChildren(const xAOD::TruthParticle &part, Ath::Object &p)
{
  std::vector<int> particles_out;

  if(part.decayVtx()) {
    p.AddVar(Ath::Def::DecayVtxBarcode, part.decayVtx()->barcode());     
    
    for(const xAOD::TruthVertex::TPLink_t &vpit: part.decayVtx()->outgoingParticleLinks()) {
      if(vpit.isValid()) {
	particles_out.push_back((*vpit)->barcode());
      }
    }
  }
  else {
    p.AddVar(Ath::Def::DecayVtxBarcode, 1.0e6);	    
  }

  p.AddVec(Ath::Def::IndexRangeChild, particles_out);
}

//=============================================================================
std::vector<const xAOD::TruthParticle *> Ath::GetTruthParents(const xAOD::TruthParticle &part)
{
  std::vector<const xAOD::TruthParticle *> outs;

  if(part.hasProdVtx()) {
    for(const xAOD::TruthVertex::TPLink_t &vpit: part.prodVtx()->incomingParticleLinks()) {
      if(vpit.isValid()) {	
	outs.push_back(*vpit);
      }
    }
  }

  return outs;
}

//=============================================================================
std::vector<const xAOD::TruthParticle *> Ath::GetTruthChildren(const xAOD::TruthParticle &part)
{
  std::vector<const xAOD::TruthParticle *> outs;

  if(part.hasDecayVtx()) {
    for(const xAOD::TruthVertex::TPLink_t &vpit: part.decayVtx()->outgoingParticleLinks()) {
      if(vpit.isValid()) {	
	outs.push_back(*vpit);
      }
    }
  }

  return outs;
}

//=============================================================================
std::vector<const xAOD::TruthParticle *> Ath::GetAllTruthChildren(const xAOD::TruthParticle &part)
{
  std::vector<const xAOD::TruthParticle *> outs;
  std::vector<const xAOD::TruthParticle *> outs_children;

  if(part.hasDecayVtx()) {
    for(const xAOD::TruthVertex::TPLink_t &vpit: part.decayVtx()->outgoingParticleLinks()) {
      if(vpit.isValid()) {	
	outs.push_back(*vpit);
	
	//
	// Recursive call to collect this particled children
	//
	const std::vector<const xAOD::TruthParticle *> children = GetAllTruthChildren(*(*vpit));

	outs_children.insert(outs_children.end(), children.begin(), children.end());
      }
    }
  }

  outs.insert(outs.end(), outs_children.begin(), outs_children.end());

  return outs;
}

//=============================================================================
std::string Ath::PrintToolProperties(IAlgTool *ialg)
{
  AlgTool *tool = dynamic_cast<AlgTool *>(ialg);

  if(!tool) {
    return "";
  }

  typedef std::map<std::string, std::string> SMap;

  unsigned swidth = 0;
  SMap smap;
   
  for(const Property *p: tool->getProperties()) {
    swidth = std::max<unsigned>(swidth, p->name().size());

    std::pair<SMap::iterator, bool> sit = smap.insert(SMap::value_type(p->name(), p->toString()));

    if(!sit.second) {
      sit.first->second += "  ---> THIS PROPERTY NAME IS DUPLICATED";
    }
  }

  std::stringstream str;
  str << "Print " <<  smap.size() << " properties of this tool: " << ialg->name() << std::endl;

  for(const SMap::value_type &s: smap) {
    str << std::setw(swidth) << std::left << s.first << ": \"" << s.second << "\"" << std::endl;
  }

  return str.str();
}
