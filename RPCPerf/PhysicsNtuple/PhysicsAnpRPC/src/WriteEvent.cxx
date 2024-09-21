
// C/C++
#include <cstdlib>
#include <climits>
#include <iomanip>

// ROOT
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

// Local
#include "Event.h"
#include "Utils.h"
#include "WriteEvent.h"

using namespace std;

//=============================================================================
// WriteEvent
//=============================================================================
Ath::WriteEvent::WriteEvent(const std::string &name):
  asg::AsgMetadataTool  (name),
  m_histSvc             ("THistSvc/THistSvc", name),
  m_tree                (0),
  m_PRWHash             (0),
  m_doFirstEventConfig  (true),
  m_inputSumEventWeight (0.0),
  m_inputSumEventWeight2(0.0),
  m_inputSumEvent       (0.0),
  m_writeSumEventWeight (0.0),
  m_writeSumEventWeight2(0.0),
  m_writeSumEvent       (0.0)
{
  declareInterface<Ath::IEventTool>(this);

  declareProperty("debug",              m_debug              = false);
  declareProperty("print",              m_print              = true);
  declareProperty("doDynamicVarConfig", m_doDynamicVarConfig = false);

  declareProperty("keys_event",         m_keys_event     = "");
  declareProperty("stream",             m_stream         = "");
  declareProperty("treename",           m_treename       = "");

  declareProperty("branches",           m_branches);
}

//=============================================================================
StatusCode Ath::WriteEvent::initialize()
{ 
  //------------------------------------------------------------------------
  // Create output tree
  //
  msg(MSG::INFO) << "PlotInit - create output tree: " << m_treename << endmsg;

  CHECK( m_histSvc.retrieve() );

  m_tree = MakeTree(m_treename, m_stream);

  //------------------------------------------------------------------------
  // Configure event branches
  //
  cout << "Configure " <<  m_keys_event.size() << " event branches" << endl; 

  ReadKeys(m_data_event, m_keys_event);

  m_tree->Branch("PRWHash", &m_PRWHash, "PRWHash/l");

  //------------------------------------------------------------------------
  // Configure object branches
  //
  for(const string &val: m_branches) {
    StrVec parts;

    StringTok(parts, val, "|");

    if(parts.size() != 2 || parts.at(0).empty()) {
      continue;
    }

    const string &branch = parts.at(0);
    DataVec &dvec = m_branchMap[branch];
    
    cout << "Configure branch - " << branch << endl;

    ReadKeys(dvec, parts.at(1));
  } 

  msg(MSG::INFO)  << "initialize" << endmsg
		  << "   debug:           " << m_debug          << endmsg
		  << "   keys_event:      " << m_keys_event     << endmsg
		  << "   stream:          " << m_stream         << endmsg
		  << "   treename:        " << m_treename       << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode Ath::WriteEvent::beginInputFile()
{
  cout << "WriteEvent::beginInputFile" << std::endl;

  return StatusCode::SUCCESS;
}

//=============================================================================
void Ath::WriteEvent::ProcEvent(Ath::Event &event)
{
  //
  // Read new event entry
  //
  msg(MSG::DEBUG) << "ProcEvent - begin..." << endmsg;

  if(!m_tree) {
    msg(MSG::DEBUG) << "PlotExec - invalid RecoEvent on null TTree pointer: " << m_tree << endmsg;
    return;
  }

  if(m_doFirstEventConfig) {
    m_doFirstEventConfig = false;  

    if(m_doDynamicVarConfig) {
      InitEvent(event);
    }

    BookBranches();
  }

  //
  // Clear previous data
  //
  for(Data &data: m_data_event) { 
    data.ClearVar(); 
    data.ReadVar(event);    
  }

  //
  // Save event data
  //
  m_PRWHash = event.GetPRWHash();

  for(BranchMap::value_type &branch: m_branchMap) {
    for(Data &data: branch.second) {
      data.ClearVec();
    }

    //
    // Read variable from vector of objects into output vector of flat variables
    //
    ReadVecData(!branch.first.empty(), branch.second,  event.GetVec(branch.first));
  }

  if(m_tree) {
    m_tree->Fill();
  }

  m_writeSumEventWeight  += event.GetMCWeight();
  m_writeSumEventWeight2 += event.GetMCWeight()*event.GetMCWeight();
  m_writeSumEvent        += 1;

  msg(MSG::DEBUG) << "ProcEvent - done" << endmsg;
}

//=============================================================================
void Ath::WriteEvent::InitEvent(Ath::Event &event)
{
  //
  // Read new event entry
  //
  msg(MSG::INFO) << "InitEvent - begin..." << endmsg;

  //
  // Configure dynamic event branches
  //
  for(const VarEntry &var: event.GetVars()) { 
    if(HasKeyInDataVec(m_data_event, var.GetKey())) {
      continue;
    }

    Data data;
    
    if(data.InitFromVarEntry(var)) {
      m_data_event.push_back(data);

      msg(MSG::INFO) << "  added new event variable:      " << Def::AsStr(var.GetKey()) << endmsg;
    }
    else{
      msg(MSG::INFO) << "  failed to initialize variable: " << Def::AsStr(var.GetKey()) << endmsg;
      continue;
    }   
  }

  //
  // Configure and book vector branches
  //
  for(BranchMap::value_type &branch: m_branchMap) {
    //
    // Configure vector branches
    //
    std::vector<Object> &objs = event.GetVec(branch.first);

    msg(MSG::INFO) << "Branch: " << branch.first << endmsg;

    for(const Object &obj: objs) {
      for(const VarEntry &var: obj.GetVars()) { 
	if(HasKeyInDataVec(branch.second, var.GetKey())) {
	  continue;
	}

	Data data;
	
	if(data.InitFromVarEntry(var)) {
	  branch.second.push_back(data);
	  
	  msg(MSG::INFO) << "  added new branch variable:     " << Def::AsStr(var.GetKey()) << endmsg;
	}
	else {
	  msg(MSG::INFO) << "  failed to initialize variable: " << Def::AsStr(var.GetKey()) << endmsg;
	  continue;
	}
      }
    }
  }

  msg(MSG::INFO) << "InitEvent - done" << endmsg;
}

//=============================================================================
void Ath::WriteEvent::BookBranches()
{
  //
  // Read new event entry
  //
  msg(MSG::INFO) << "BookBranches - begin..." << endmsg;

  //
  // Book event branches
  //
  for(Data &data: m_data_event) {     
    data.BookVarBranch(m_tree); 
  }

  //
  // Configure and book vector branches
  //
  for(BranchMap::value_type &branch: m_branchMap) {

    for(Data &data: branch.second) {
      data.BookVecBranch(m_tree, branch.first);
    }
  }

  msg(MSG::INFO) << "BookBranches - done" << endmsg;
}

//=============================================================================
StatusCode Ath::WriteEvent::finalize()
{
  const std::string hname = m_treename + "_Count";

  TH1 *h_event_count = new TH1D(hname.c_str(), hname.c_str(), 4, 0.0, 4.0);
  h_event_count->GetXaxis()->SetBinLabel(1, "In event count");
  h_event_count->GetXaxis()->SetBinLabel(2, "In event weight");
  h_event_count->GetXaxis()->SetBinLabel(3, "Out event count");
  h_event_count->GetXaxis()->SetBinLabel(4, "Out event weight");
  
  h_event_count->SetBinContent(1, m_inputSumEvent);
  h_event_count->SetBinContent(2, m_inputSumEventWeight);

  h_event_count->SetBinContent(3, m_writeSumEvent);
  h_event_count->SetBinContent(4, m_writeSumEventWeight);

  if(!(m_inputSumEventWeight2 < 0.0)) {
    h_event_count->SetBinError(2, std::sqrt(m_inputSumEventWeight2));
  }
  if(!(m_writeSumEventWeight2 < 0.0)) {
    h_event_count->SetBinError(4, std::sqrt(m_writeSumEventWeight2));
  }

  RegHist(h_event_count, "");
  
  return StatusCode::SUCCESS;
}

//=============================================================================
void Ath::WriteEvent::ReadKeys(std::vector<Data> &dvec, const std::string &val) const
{
  //
  // Read keys for writting out variables into TTree branches
  //
  vector<string> keys;
  Ath::StringTok(keys, val, ", ");

  cout << "WriteEvent::ReadKeys - process " << keys.size() << endl;

  unsigned nwidth = 0;
  
  for(const string &key: keys) {
    VarConfig vc;
    
    if(!vc.ConfigVarFromStr(key)) {
      throw std::logic_error("WriteEvent::ReadKeys - failed to configure variable: \"" + key + "\"");
    }

    Data data;
    data.InitFromVarConfig(vc);
    
    dvec.push_back(data);

    nwidth = std::max<unsigned>(nwidth, data.name.size());
    nwidth = std::max<unsigned>(nwidth, data.nick.size());
  }

  for(const Data &data: dvec) {
    cout << "  name=\"" << setw(nwidth) << std::left << data.name << "\""
	 << "  nick=\"" << setw(nwidth) << std::left << data.nick << "\""
	 << "  type=\"" << Ath::GetTypeAsStr(data.type) << "\""
	 << endl; 
  }
}

//=============================================================================
void Ath::WriteEvent::PrintKeys(const std::vector<Data> &dvec, const std::string &name)
{
  cout << "PrintKeys - print " << setw(3) << dvec.size() << " key(s) for: " << name << endl;

  for(const Data &data: dvec) {
    cout << "   "     << setw(20) << left << Def::AsStr(data.var)
	 << " type: " << setw(10) << left << Ath::GetTypeAsStr(data.type)
	 << " branch: " << data.nick << endl;
  }
}

//=============================================================================
TTree* Ath::WriteEvent::MakeTree(const std::string &tree_name, 
					const std::string &stream_name)
{
  //
  // Make and register TTree via THistSvc
  //
  if(tree_name.empty() || stream_name.empty()) { 
    return 0;
  }

  const std::string tree_key = "/"+stream_name+"/"+tree_name;  
  TTree *tree = 0;

  if(m_histSvc->exists(tree_key) && m_histSvc->getTree(tree_key, tree).isSuccess()) {
    msg() << MSG::INFO << "Retrieved TTree from HistSvc: " << tree_key << endmsg;    
    return tree;
  }

  tree = new TTree(tree_name.c_str(), tree_name.c_str());
  tree -> SetDirectory(0);
  
  if(m_histSvc -> regTree(tree_key, tree).isFailure()) {
    msg() << MSG::WARNING << "Could not register TTree " << tree_name << endmsg;
    delete tree; tree = 0;
  }
  else {
    msg() << MSG::INFO << "Registered new TTree " << tree_key << endmsg;
  }

  return tree;
}

//=============================================================================
void Ath::WriteEvent::RegHist(TH1 *h, const std::string &key)
{
  //
  // Register histogram
  //
  std::string hist_key;

  if(key.empty() && h) {
    hist_key = "/" + m_stream + "/" + std::string(h->GetName());
  }
  else {
    hist_key = "/" + m_stream + "/" + key;
  }
  
  if(m_histSvc->regHist(hist_key, h).isFailure()) {
    msg(MSG::WARNING) << "Could not register histogram: " << hist_key << endmsg;
  }
  else {
    msg( MSG::INFO) << "Registered histogram: " << hist_key << endmsg;
  }
}
