// OutputMaker includes
#include "OutputMaker_Hists.h"

//#include "xAODEventInfo/EventInfo.h"

//uncomment the line below to use the HistSvc for outputting trees and histograms
//#include "GaudiKernel/ITHistSvc.h"
//#include "TTree.h"
//#include "TH1D.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"

OutputMaker_Hists::OutputMaker_Hists( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAnalysisAlgorithm( name, pSvcLocator ),
    m_grl("GoodRunsListSelectionTool/GoodRunsListSelectionTool"),
    m_helper("METTriggerHelper/METTriggerHelper"),
    m_trigMets({HLTMET::Cell,
                HLTMET::Topocl,
                HLTMET::Topocl_PS,
                HLTMET::Topocl_PUC,
                HLTMET::mht,
                HLTMET::L1}),
    m_hltOnlyTrigMets({HLTMET::Cell,
                       HLTMET::Topocl,
                       HLTMET::Topocl_PS,
                       HLTMET::Topocl_PUC,
                       HLTMET::mht}),
    GeV(0.001),
    m_lowestUnprescaledCuts({ {HLTMET::Cell, 100},
                              {HLTMET::Topocl, 80},
                              {HLTMET::Topocl_PS, 120},
                              {HLTMET::Topocl_PUC, 120},
                              {HLTMET::mht, 90},
                              {HLTMET::L1, 50} }),
    m_equalRateCuts({ {HLTMET::Cell, 70},
                      {HLTMET::Topocl, 84},
                      {HLTMET::Topocl_PS, 87},
                      {HLTMET::Topocl_PUC, 88},
                      {HLTMET::mht, 87},
                      {HLTMET::L1, 50} })
{
  declareProperty( "UseGRL", m_useGRL = true );
  declareProperty( "Signals", m_signals );
  declareProperty( "RemoveSoftTerm", m_noSoft = false );
  declareProperty( "UseCST", m_useCST = false );
  declareProperty( "StoreGatePrefix", m_sgPrefix );
  declareProperty( "DirName", m_dirName );
  //declareProperty( "Property", m_nProperty ); //example property declaration

}


OutputMaker_Hists::~OutputMaker_Hists() {}


StatusCode OutputMaker_Hists::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");

  if (m_noSoft && m_useCST) {
    ATH_MSG_ERROR( "Both RemoveSoftTerm and UseCST specified! Only one can be set to true" );
    return StatusCode::FAILURE;
  }

  if (m_useGRL) ATH_CHECK( m_grl.retrieve() );

  ATH_CHECK( m_helper.retrieve() );

  //m_doSignalSelection = m_signal != "None";
  for (const std::string& signal : m_signals) {
    m_cacc_signals.emplace_back("Pass_"+signal);
  }

  float metLow = 0;
  float metHigh = 400;
  int metNBins = 400;

  float metXLow = -300;
  float metXHigh = 300;
  int metXNBins = 300;

  float sumEtLow = 0;
  float sumEtHigh = 800;
  int sumEtNBins = 160;

  std::string metTitle = "E_{T}^{miss}";
  std::string refTitle = metTitle + " (offline, no muons, ";
  std::string metXTitle = "E_{x}^{miss}";
  std::string refXTitle = metXTitle + " (offline, no muons, ";
  std::string metYTitle = "E_{y}^{miss}";
  std::string refYTitle = metYTitle + " (offline, no muons, ";
  std::string sumEtTitle = "#sum E_{T}";
  std::string refSumEtTitle = sumEtTitle + " (offline, no muons, ";
  if (m_noSoft) {
    refTitle += "no TST";
    refXTitle += "no TST";
    refYTitle += "no TST";
    refSumEtTitle += "no TST";
  }
  else if (m_useCST) {
    refTitle += "CST";
    refXTitle += "CST";
    refYTitle += "CST";
    refSumEtTitle += "CST";
  }
  else {
    refTitle += "TST";
    refXTitle += "TST";
    refYTitle += "TST";
    refSumEtTitle += "TST";
  }
  refTitle += ") [GeV]";
  refXTitle += ") [GeV]";
  refYTitle += ") [GeV]";
  refSumEtTitle += ") [GeV]";
  std::string state;
  for (const HLTMET& alg : m_hltOnlyTrigMets) { // {z:L1} vs {y:HLT} vs {x:off}
    state = "TH3s";
    std::string algName = m_helper->getHLTMETName(alg);
    TH3* h3 = new TH3F( ("L1_v_"+algName+"_v_off").c_str(), (algName+";"+refTitle+";"+metTitle+" ("+algName+") [GeV];"+metTitle+" (L1) [GeV]").c_str(), metNBins, metLow, metHigh, metNBins, metLow, metHigh, metNBins, metLow, metHigh);
    h3->Sumw2();
    m_th3s[h3->GetName()] = h3;
  }
  for (const auto& pair : m_th3s) {
    ATH_CHECK(histSvc()->regHist(m_dirName+"/"+pair.first, pair.second) );
  }
#if 0
  // quick turn on material
  ATH_CHECK(book(TH1F("reference", (";"+refTitle).c_str(), metNBins, metLow, metHigh) ) );
  hist("reference")->Sumw2();
  for (const HLTMET& alg : m_trigMets) {
    std::string algName = m_helper->getHLTMETName(alg);
    ATH_MSG_DEBUG( "Initialising histgorams for " << algName );
    std::string triggerName;
    state = triggerName + " equalRates";
    if (alg == HLTMET::L1) triggerName = m_helper->getTriggerName(alg, m_equalRateCuts.at(alg) );
    else triggerName = m_helper->getTriggerName(alg, m_equalRateCuts.at(alg), m_equalRateCuts.at(HLTMET::L1) );
    ATH_CHECK(book(TH1F( (algName+"_equalRate").c_str(), (triggerName+";"+refTitle).c_str(), metNBins, metLow, metHigh) ) );
    hist(algName+"_equalRate")->Sumw2();
    state = triggerName + " lowestUnprescaled";
    if (alg == HLTMET::L1) triggerName = m_helper->getTriggerName(alg, m_lowestUnprescaledCuts.at(alg) );
    else triggerName = m_helper->getTriggerName(alg, m_lowestUnprescaledCuts.at(alg), m_lowestUnprescaledCuts.at(HLTMET::L1) );
    ATH_CHECK(book(TH1F( (algName+"_lowestUnprescaled").c_str(), (triggerName+";"+refTitle).c_str(), metNBins, metLow, metHigh) ) );
    hist(algName+"_lowestUnprescaled")->Sumw2();
    state = "linearity";
    // linearity
    ATH_CHECK(book(TH3F( (algName+"_lin").c_str(), (algName+";"+refSumEtTitle+";"+refTitle+";"+metTitle+" ("+algName+") [GeV]").c_str(), sumEtNBins, sumEtLow, sumEtHigh, metNBins, metLow, metHigh, metNBins, metLow, metHigh) ) );
    hist3d(algName+"_lin")->Sumw2();
    // resolution
    state = "resolution";
    ATH_CHECK(book(TH3F( (algName+"_resx").c_str(), (algName+"_x;"+refSumEtTitle+";"+refXTitle+";"+metXTitle+" ("+algName+") [GeV]").c_str(), sumEtNBins, sumEtLow, sumEtHigh, metXNBins, metXLow, metXHigh, metXNBins, metXLow, metXHigh) ) );
    hist3d(algName+"_resx")->Sumw2();
    ATH_CHECK(book(TH3F( (algName+"_resy").c_str(), (algName+"_y;"+refSumEtTitle+";"+refYTitle+";"+metYTitle+" ("+algName+") [GeV]").c_str(), sumEtNBins, sumEtLow, sumEtHigh, metXNBins, metXLow, metXHigh, metXNBins, metXLow, metXHigh) ) );
    hist3d(algName+"_resy")->Sumw2();
  }
#endif
  return StatusCode::SUCCESS;
}

StatusCode OutputMaker_Hists::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode OutputMaker_Hists::execute() {  
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

  const xAOD::MissingETContainer* metCont;
  ATH_CHECK( evtStore()->retrieve(metCont, m_sgPrefix + "EtMissNoMuAux") );

  std::string finalTerm = m_useCST ? "FinalClus" : "FinalTrk";

  auto metItr = metCont->find(finalTerm);
  if (metItr == metCont->end() ) {
    ATH_MSG_ERROR( finalTerm << " not present in container " << m_sgPrefix << "EtMissNoMuAux" );
    return StatusCode::FAILURE;
  }
  float metx = (*metItr)->mpx() * GeV;
  float mety = (*metItr)->mpy() * GeV;
  float sumEt = (*metItr)->sumet() * GeV;

  if (m_noSoft) {
    metItr = metCont->find("PVSoftTrk");
    if (metItr == metCont->end() ) {
      ATH_MSG_ERROR( "PVSoftTrk not present in container " << m_sgPrefix << "EtMissNoMuAux" );
      return StatusCode::FAILURE;
    }
    metx -= (*metItr)->mpx() * GeV;
    mety -= (*metItr)->mpy() * GeV;
    sumEt -= (*metItr)->sumet() * GeV;
  }
  float met = sqrt(metx*metx + mety*mety);
#if 0
  TH1* h1 = hist("reference");
  if (!h1) return StatusCode::FAILURE;
  h1->Fill(met);
  std::map<HLTMET, float> trigMetValues;
  std::map<HLTMET, float> trigMetXValues;
  std::map<HLTMET, float> trigMetYValues;
  //std::map<HLTMET, float> trigSumEtValues;
  for (const HLTMET& alg : m_trigMets) {
    std::string algName = m_helper->getHLTMETName(alg);
    ATH_CHECK( m_helper->getMET(alg, trigMetValues[alg]) );
    ATH_CHECK( m_helper->getMETX(alg, trigMetXValues[alg]) );
    ATH_CHECK( m_helper->getMETY(alg, trigMetYValues[alg]) );
    //ATH_CHECK( m_helper->getSumEt(alg, trigSumEtValues[alg]) );
    if (trigMetValues.at(alg) > m_equalRateCuts.at(alg) && trigMetValues.at(HLTMET::L1) > m_equalRateCuts.at(HLTMET::L1) ) {
      h1 = hist(algName+"_equalRate");
      if (!h1) return StatusCode::FAILURE;
      h1->Fill(met);
    }
    if (trigMetValues.at(alg) > m_lowestUnprescaledCuts.at(alg) && trigMetValues.at(HLTMET::L1) > m_lowestUnprescaledCuts.at(HLTMET::L1) ) {
      h1 = hist(algName+"_lowestUnprescaled");
      if (!h1) return StatusCode::FAILURE;
      h1->Fill(met);
    }
    TH3* h3 = hist3d(algName+"_lin");
    if (!h3) return StatusCode::FAILURE;
    h3->Fill(sumEt, met, trigMetValues.at(alg) );

    h3 = hist3d(algName+"_resx");
    if (!h3) return StatusCode::FAILURE;
    h3->Fill(sumEt, metx, trigMetXValues.at(alg) );

    h3 = hist3d(algName+"_resy");
    if (!h3) return StatusCode::FAILURE;
    h3->Fill(sumEt, mety, trigMetYValues.at(alg) );
  }
#endif
  float L1MET;
  ATH_CHECK( m_helper->getMET(HLTMET::L1, L1MET) );
  for (const HLTMET& alg : m_hltOnlyTrigMets) {
    float trigMet;
    ATH_CHECK( m_helper->getMET(alg, trigMet) );
    std::string algName = m_helper->getHLTMETName(alg);
    TH3* h3 = m_th3s.at("L1_v_"+algName+"_v_off");
    h3->Fill(met, trigMet, L1MET);
  }

  return StatusCode::SUCCESS;
}

StatusCode OutputMaker_Hists::beginInputFile() { 
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


