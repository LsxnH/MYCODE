// C/C++
#include <set>
#include <iostream>
#include <fstream>

// ROOT
#include "TH1.h"

// Local
#include "CutFlow.h"

using namespace std;

//==============================================================================
Ath::CutFlow::CutFlow():
  fDebug (false),
  fNInput(0)
{
}

//==============================================================================
void Ath::CutFlow::ConfCuts(const std::vector<std::string> &confs, const std::string &caller, bool print)
{
  //
  // Configure cuts
  //
  std::cout << caller << " - configure " << confs.size() << " cuts" << std::endl;

  for(const std::string &conf: confs) {
    std::cout << "   " << conf << std::endl;
    AddCut(conf);
  }

  if(print) {
    PrintConf(std::cout, "   ");
  }
}

//==============================================================================-
std::ostream& Ath::CutFlow::log() const
{
  std::cout << "CutFlow::";
  return std::cout;
}

//==============================================================================
void Ath::CutFlow::AddCut(const std::string &conf)
{
  //
  // Add and configure new cut
  //
  fCuts.push_back(CutPair());
  fCuts.back().icut.InitCut(conf);
}

//==============================================================================
void Ath::CutFlow::PrintConf(std::ostream &os, const std::string &pad) const
{
  //
  // Print configuration
  //
  os << pad << "Number of cuts: " << fCuts.size() << endl;

  for(const CutPair &p: fCuts) {
    p.icut.PrintConfig(os, pad + "   ");   
  }
}

//==============================================================================
TH1* Ath::CutFlow::MakeCutFlowHist(const std::string &key) const
{
  //
  // Save histogram with cut-flows
  //
  TH1D *hc = new TH1D(key.c_str(), key.c_str(), fCuts.size()+1, 0, fCuts.size()+1);
  hc->SetDirectory(0);
  hc->SetStats(false);   
  hc->GetXaxis()->SetBinLabel(1, "Input");

  hc->SetBinContent(1, fNInput);

  for(unsigned i = 0; i < fCuts.size(); ++i) {
    const CutPair &p = fCuts.at(i);

    const uint bin = i+2;
    double val =           p.npass;
    double err = std::sqrt(p.npass);
    
    hc->SetBinContent(bin, val);
    hc->SetBinError  (bin, err);
    hc->GetXaxis()->SetBinLabel(bin, p.icut.GetName().c_str());
  }

  return hc;
}

//==============================================================================
void Ath::CutFlow::PrintCuts(std::ostream &os, const std::string &pad) const
{
  //
  // Print cutflow table for sets of samples, first compute name width
  //
  unsigned namew = string("Input").size();

  for(unsigned i = 0; i < fCuts.size(); ++i) {
    const CutPair &c = fCuts.at(i);

    //
    // Find maximum width for cut name
    //
    namew = std::max<unsigned>(namew, c.icut.GetName().size());
  }

  os << pad << setw(namew) << left << "Input" << " " << fNInput << "\n";

  for(const CutPair &p: fCuts) {
    os << pad << setw(namew)  << left  << p.icut.GetName() << " " << right << p.npass << "\n";
  }
}

