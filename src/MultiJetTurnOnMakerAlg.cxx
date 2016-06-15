// MultiJetTurnOnMaker includes
#include "MultiJetTurnOnMakerAlg.h"

//#include "xAODEventInfo/EventInfo.h"

#include <iostream>

//uncomment the line below to use the HistSvc for outputting trees and histograms
//#include "GaudiKernel/ITHistSvc.h"
//#include "TTree.h"
//#include "TH1D.h"

#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "xAODJet/JetContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"

MultiJetTurnOnMakerAlg::MultiJetTurnOnMakerAlg( const std::string& name, ISvcLocator* pSvcLocator )
  : AthHistogramAlgorithm( name, pSvcLocator ),
    firstEvent(true),
    m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool.h"),
    GeV(0.001)
{
  declareProperty("UseGRL", m_useGRL = true);
  declareProperty("Signals", m_signals );
  declareProperty("StoreGatePrefix", m_sgPrefix);
  declareProperty("Decorator", m_decorator);
  declareProperty("DirName", m_dirName);
  //declareProperty("RefrenceTriggers", m_refTriggers);
  //declareProperty( "Property", m_nProperty ); //example property declaration
  //declareProperty( "IsData", m_isData);
}


MultiJetTurnOnMakerAlg::~MultiJetTurnOnMakerAlg() {}


StatusCode MultiJetTurnOnMakerAlg::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  for (const std::string& signal : m_signals) {
    m_cacc_signals.emplace_back("Pass_"+signal);
  }
  //m_objTool.declarePropertyFor(this, "ObjTool");
  ATH_CHECK(m_trigDecTool.retrieve() );
  int nBins = 400;
  float low = 0;
  float high = 400;

  for (const std::string& trig : {"HLT_4j85", "HLT_4j85_jes", "HLT_4j85_lcw", "HLT_4j85_lcw_jes", "HLT_4j85_lcw_nojcalib", "HLT_4j85_nojcalib"} ) {
    TH2* h2 = new TH2F(trig.c_str(), (trig+"; 4th Jet pT [GeV];Pass Trigger").c_str(), nBins, low, high, 2, 0, 2);
    m_th2s[trig] = h2;
    h2->Sumw2();
  }
  for (const std::string& trig : {"HLT_5j45", "HLT_5j55", "HLT_5j60", "HLT_5j850", "HLT_5j70", "HLT_5j70_L14J15"} ) {
    TH2* h2 = new TH2F(trig.c_str(), (trig + "; 5th Jet pT [GeV]").c_str(), nBins, low, high, 2, 0, 2);
    m_th2s[trig] = h2;
    h2->Sumw2();
  }
  for (const std::string& trig : {"HLT_5j65_0eta240", "HLT_5j70_0eta240"} ) {
    TH2* h2 = new TH2F(trig.c_str(), (trig+"; 5th Jet (0 #leq |#eta| #leq 2.4) pT [GeV]").c_str(), nBins, low, high, 2, 0, 2);
    m_th2s[trig] = h2;
    h2->Sumw2();
  }
  for (const std::string& trig : {"HLT_6j45_0eta240", "HLT_6j50_0eta240_L14J15"} ) {
    TH2* h2 = new TH2F(trig.c_str(), (trig+"; 6th jet (0 #leq |#eta| #leq 2.4) pT [GeV]").c_str(), nBins, low, high, 2, 0, 2);
    m_th2s[trig] = h2;
    h2->Sumw2();
  }

  for (const std::string& trig : {"HLT_7j45", "HLT_7j45_L14J15", "HLT_7j45_L15J50ETA25"}) {
    TH2* h2 = new TH2F(trig.c_str(), (trig+"; 7th Jet pT [GeV]").c_str(), nBins, low, high, 2, 0, 2);
    m_th2s[trig] = h2;
    h2->Sumw2();
  }

  for (const std::string& trig : {"HLT_j15", "HLT_j25", "HLT_j60", "HLT_j85", "HLT_j110", "HLT_j150", "HLT_j175", "HLT_j200", "HLT_j260", "HLT_j320", "HLT_j360", "HLT_j400"} ) {
    TH2* h2 = new TH2F(trig.c_str(), (trig+"; 1st Jet pT [GeV]").c_str(), 600, 0, 600, 2, 0, 2);
    m_th2s[trig] = h2;
    h2->Sumw2();
  }

  for (const auto& pair : m_th2s) {
    ATH_CHECK(histSvc()->regHist(m_dirName+"/"+pair.first, pair.second) );
  }

  //for (const std::string& trig : {"HLT_xe100_L1XE50", "HLT_xe90_mht_L1XE50", "HLT_xe110_pueta_L1XE50", "HLT_xe110_pufit_L1XE50", "HLT_xe100_tc_em_L1XE50", "HLT_xe80_tc_lcw_L1XE50", "HLT_xe90_tc_lcw_L1XE50", "HLT_xe90_tc_lcw_wEFMu_L1XE50"} ) {
    //ATH_CHECK( book(TH1F(trig.c_str(), (trig + "; E_{T}^{miss} (offline) [GeV]").c_str(), 400, 0, 400) ) );
  //}
  //ATH_CHECK( book(TH1F("ref_met", "; E_{T}^{miss} (offline) [GeV]", 400, 0, 400) ) );
  //
  //This is called once, before the start of the event loop
  //Retrieves of tools you have configured in the joboptions go here
  //

  //HERE IS AN EXAMPLE
  //We will create a histogram and a ttree and register them to the histsvc
  //Remember to uncomment the configuration of the histsvc stream in the joboptions
  //
  //ServiceHandle<ITHistSvc> histSvc("THistSvc",name());
  //TH1D* myHist = new TH1D("myHist","myHist",10,0,10);
  //CHECK( histSvc->regHist("/MYSTREAM/myHist", myHist) ); //registers histogram to output stream (like SetDirectory in EventLoop)
  //TTree* myTree = new TTree("myTree","myTree");
  //CHECK( histSvc->regTree("/MYSTREAM/SubDirectory/myTree", myTree) ); //registers tree to output stream (like SetDirectory in EventLoop) inside a sub-directory


  //for (const std::string& trigName : triggerStrings) {
    //nJetPts.insert(std::atoi(trigName.substr(4,1).c_str() ) );
    //ATH_CHECK( book(TH1F(trigName.c_str(), (trigName + " turn on; " + trigName.substr(4,1) + " jet pT [GeV]").c_str(), nBins, lowPt, highPt) ) );
    //ATH_CHECK( book(TH1F((trigName+"_total").c_str(), (trigName + " total; " + trigName.substr(4,1) + " jet pT [GeV]").c_str(), nBins, lowPt, highPt) ) );
  //}
  //for (int nJet : nJetPts) {
    //ATH_CHECK( book(TH1F(("h_"+std::to_string(nJet)+"jetPt").c_str(), (std::to_string(nJet) + " jet pT;" + std::to_string(nJet) + " jet pT [GeV]").c_str(), nBins, lowPt, highPt) ) );
  //}
  //ATH_CHECK( book(TH1F("cutflow", "", 1, 0, 1) ) );
  //ATH_CHECK( book(TH2F("ptVeta", ";Jet p_{T} [GeV]; Jet #eta", 100, 0, 200, 100, -5, 5) ) );

  //hist("cutflow")->Fill("All Events", 0);
  //hist("cutflow")->Fill("Has Primary Vertex", 0);
  //hist("cutflow")->Fill("reference triggers", 0);
  //hist("cutflow")->Fill("data quality requirements", 0);
  //if (m_mode == "electron" || m_mode == "electronNoOR") hist("cutflow")->Fill("Has Electron", 0);
  //if (m_mode == "muon" || m_mode == "muonNoOR") hist("cutflow")->Fill("Has Muon", 0);
  //if (m_mode == "lepton" || m_mode == "leptonNoOR") hist("cutflow")->Fill("Has Lepton", 0);
  //if (m_mode == "lepton" || m_mode == "electron")  hist("cutflow")->Fill("Has Electron", 0);

  return StatusCode::SUCCESS;
}

StatusCode MultiJetTurnOnMakerAlg::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //

  hist("cutflow")->LabelsDeflate("X");

  return StatusCode::SUCCESS;
}

#include <sstream>

StatusCode MultiJetTurnOnMakerAlg::execute() {
  ATH_MSG_DEBUG ("Executing " << name() << "...");

  const xAOD::EventInfo* evtInfo(0);
  ATH_CHECK( evtStore()->retrieve(evtInfo, "EventInfo") );

  if (m_cacc_signals.size() > 0) {
    bool passEvent = false;
    for (auto& cacc : m_cacc_signals) {
      if (cacc(*evtInfo) ) {
        passEvent = true;
        break;
      }
    }
    if (!passEvent) return StatusCode::SUCCESS;
  }

  if (m_useGRL && !m_grl->passRunLB(*evtInfo) ) return StatusCode::SUCCESS;


  const xAOD::JetContainer* jets(0);
  ATH_CHECK( evtStore()->retrieve(jets, m_sgPrefix + "Jets") );

  std::vector<float> jetPt;
  std::vector<float> jetPt0eta240;
  //std::vector<float> jetPtNoOR;
  //std::vector<float> jetPt0eta240NoOR;
  static SG::AuxElement::ConstAccessor<char> cacc_passOR(m_decorator+":passOR");
  static SG::AuxElement::ConstAccessor<char> cacc_signal(m_decorator);
  for (const xAOD::Jet* ijet : *jets) {
    if (cacc_passOR(*ijet) ) {
      float pT = ijet->pt() * 0.001;
      float fabsEta = fabs(ijet->eta() );
      jetPt.push_back(pT);
      if (fabsEta >= 0 && fabsEta <= 2.4) {
        jetPt0eta240.push_back(pT);
      }
    }
  }
  if (jetPt.size() >= 4) {
    for (const std::string& trig : {"HLT_4j45", "HLT_4j85", "HLT_4j85_jes", "HLT_4j85_lcw", "HLT_4j85_lcw_jes", "HLT_4j85_lcw_nojcalib", "HLT_4j85_nojcalib"} ) {
      m_th2s.at(trig)->Fill(jetPt.at(3), 0);
      if (m_trigDecTool->isPassed(trig) ) m_th2s.at(trig)->Fill(jetPt.at(3), 1);
    }
  }
  if (jetPt.size() >= 5) {
    for (const std::string& trig : {"HLT_5j45", "HLT_5j55", "HLT_5j60", "HLT_5j850", "HLT_5j70", "HLT_5j70_L14J15"} ) {
      m_th2s.at(trig)->Fill(jetPt.at(4), 0);
      if (m_trigDecTool->isPassed(trig) ) hist(trig)->Fill(jetPt.at(4), 1);
    }
  }
  if (jetPt0eta240.size() >= 5) {
    for (const std::string& trig : {"HLT_5j65_0eta240", "HLT_5j70_0eta240"} ) {
      m_th2s.at(trig)->Fill(jetPt0eta240.at(4), 0);
      if (m_trigDecTool->isPassed(trig) ) m_th2s.at(trig)->Fill(jetPt0eta240.at(4), 1);
    }
  }
  if (jetPt0eta240.size() >= 6) {
    for (const std::string& trig : {"HLT_6j45_0eta240", "HLT_6j50_0eta240_L14J15"} ) {
      m_th2s.at(trig)->Fill(jetPt0eta240.at(5), 0);
      if (m_trigDecTool->isPassed(trig) ) m_th2s.at(trig)->Fill(jetPt0eta240.at(5), 1);
    }
  }
  if (jetPt.size() >= 7) {
    for (const std::string& trig : {"HLT_7j45", "HLT_7j45_L14J15", "HLT_7j45_L15J50ETA25"}) {
      m_th2s.at(trig)->Fill(jetPt.at(6), 0);
      if (m_trigDecTool->isPassed(trig) ) m_th2s.at(trig)->Fill(jetPt.at(6), 1);
    }
  }

  if (jetPt.size() >= 1) {
    for (const std::string& trig : {"HLT_j15", "HLT_j25", "HLT_j60", "HLT_j85", "HLT_j110", "HLT_j150", "HLT_j175", "HLT_j200", "HLT_j260", "HLT_j320", "HLT_j360", "HLT_j400"} ) {
      m_th2s.at(trig)->Fill(jetPt.at(0), 0);
      if (m_trigDecTool->isPassed(trig) ) m_th2s.at(trig)->Fill(jetPt.at(0), 1);
    }
  }
  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode MultiJetTurnOnMakerAlg::beginInputFile() { 
  //
  //This method is called at the start of each input file, even if
  //the input file contains no events. Accumulate metadata information here
  //

  //example of retrieval of CutBookkeepers: (remember you will need to include the necessary header files and use statements in requirements file)
  // const xAOD::CutBookkeeperContainer* bks = 0;
  // CHECK( inputMetaStore()->retrieve(bks, "CutBookkeepers") );

  //example of IOVMetaData retrieval (see https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_access_file_metadata_in_C)
  //float beamEnergy(0); CHECK( retrieveMetadata("/TagInfo","beam_energy",beamEnergy) );
  //std::vector<float> bunchPattern; CHECK( retrieveMetadata("/Digitiation/Parameters","BeamIntensityPattern",bunchPattern) );



  return StatusCode::SUCCESS;
}


