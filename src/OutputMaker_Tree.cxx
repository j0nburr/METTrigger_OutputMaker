// OutputMaker includes
#include "OutputMaker_Tree.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODMissingET/MissingETContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODEgamma/PhotonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODTrigger/EnergySumRoI.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCore/ShallowCopy.h"

#include "AthLinks/ElementLink.h"

//uncomment the line below to use the HistSvc for outputting trees and histograms
//#include "GaudiKernel/ITHistSvc.h"
//#include "TTree.h"
//#include "TH1D.h"

namespace MatchedParticleType {
  enum partType {
    Other = 0, ///< An object not falling into any of the other categories

    // Reconstructed particle types
    // {

    CaloCluster  = 1, ///< The object is a calorimeter cluster
    Jet          = 2, ///< The object is a jet
    ParticleFlow = 3, ///< The object is a particle-flow object

    TrackParticle   = 4, ///< The object is a charged track particle
    NeutralParticle = 5, ///< The object is a neutral particle

    Electron = 6, ///< The object is an electron
    Photon   = 7, ///< The object is a photon
    Muon     = 8, ///< The object is a muon
    Tau      = 9, ///< The object is a tau (jet)

    BadJet = 12,
    PileupJet = 22
  };
}

int getMatchedParticleType(const xAOD::IParticle* originalParticle) {
  static SG::AuxElement::ConstAccessor<ElementLink<xAOD::IParticleContainer>> cacc_link("MatchedRecoObject");
  const ElementLink<xAOD::IParticleContainer> link = cacc_link(*originalParticle);
  if (!link.isValid() ) return MatchedParticleType::Other;
  const xAOD::IParticle& matchedParticle = **link;
  switch( matchedParticle.type() ) {
    case xAOD::Type::CaloCluster:
      return MatchedParticleType::CaloCluster;
    case xAOD::Type::Jet:
      static SG::AuxElement::ConstAccessor<char> cacc_passCleaning("passCleaning");
      if (!cacc_passCleaning(matchedParticle) ) return MatchedParticleType::BadJet;
      static SG::AuxElement::ConstAccessor<char> cacc_pileupJet("pileupJet");
      if (cacc_pileupJet(matchedParticle) ) return MatchedParticleType::PileupJet;
      return MatchedParticleType::Jet;
    case xAOD::Type::ParticleFlow:
      return MatchedParticleType::ParticleFlow;
    case xAOD::Type::TrackParticle:
      return MatchedParticleType::TrackParticle;
    case xAOD::Type::NeutralParticle:
      return MatchedParticleType::NeutralParticle;
    case xAOD::Type::Electron:
      return MatchedParticleType::Electron;
    case xAOD::Type::Photon:
      return MatchedParticleType::Photon;
    case xAOD::Type::Muon:
      return MatchedParticleType::Muon;
    case xAOD::Type::Tau:
      return MatchedParticleType::Tau;
    default:
      break;
  }
  return MatchedParticleType::Other;
}

OutputMaker_Tree::OutputMaker_Tree( const std::string& name, ISvcLocator* pSvcLocator ) 
  : AthAnalysisAlgorithm( name, pSvcLocator ),
    m_grl("GoodRunsListSelectionTool/GoodRunsListSelectionTool"),
    m_helper("METTriggerHelper/METTriggerHelper"),
    m_trigDecTool("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_bcTool("Trig::TrigConfBunchCrossingTool/BunchCrossingTool"),
    m_trigObjMatchTool("TrigRecoObjectMatchingTool/TriggerMatchingTool"),
    clusterContainerNames({"HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker_slw",
                           "HLT_xAOD__CaloClusterContainer_TrigEFCaloCalibFex",
                           "HLT_xAOD__CaloClusterContainer_TrigCaloClusterMaker",
                           "CaloCalTopoClusters"}),
    trigJetContainerNames({"HLT_xAOD__JetContainer_a4tcemjesPS",
                           "HLT_xAOD__JetContainer_a4tcemsubFS",
                           "HLT_xAOD__JetContainer_a4tcemjesFS",
                           "HLT_xAOD__JetContainer_a4tcemnojcalibFS",
                           "HLT_xAOD__JetContainer_a4tclcwnojcalibFS",
                           "HLT_xAOD__JetContainer_a4tclcwjesFS",
                           "HLT_xAOD__JetContainer_a4tcemsubjesFS",
                           "HLT_xAOD__JetContainer_a4tclcwsubjesFS"}),
#if 0
    trigJetContainerNames({"HLT_xAOD__JetContainer_a4tclcwsubFS",
                           "HLT_xAOD__JetContainer_a4tclcwjesPS",
                           "HLT_xAOD__JetContainer_a4tclcwnojcalibPS",
                           "HLT_xAOD__JetContainer_a4TThadnojcalibFS",
                           "HLT_xAOD__JetContainer_a4tcemnojcalibPS",
                           "HLT_xAOD__JetContainer_a4TTemnojcalibFS",
                           "HLT_xAOD__JetContainer_a4tcemjesPS",
                           "HLT_xAOD__JetContainer_a4tcemsubFS",
                           "HLT_xAOD__JetContainer_a4tcemjesFS",
                           "HLT_xAOD__JetContainer_a4tcemnojcalibFS",
                           "HLT_xAOD__JetContainer_a4tclcwnojcalibFS",
                           "HLT_xAOD__JetContainer_a4tclcwjesFS",
                           "HLT_xAOD__JetContainer_a4tcemsubjesFS",
                           "HLT_xAOD__JetContainer_a4tclcwsubjesFS"}),
#endif
    GeV(0.001)
{
  declareProperty( "UseGRL", m_useGRL );
  declareProperty( "Signals", m_signals );
  declareProperty( "Triggers", m_triggers );
  declareProperty( "Decorator", m_decorator );
  declareProperty( "StoreGatePrefix", m_sgPrefix );
  declareProperty( "BCTool", m_bcTool );
  //declareProperty( "Property", m_nProperty ); //example property declaration

}


OutputMaker_Tree::~OutputMaker_Tree() {}


StatusCode OutputMaker_Tree::initialize() {
  ATH_MSG_INFO ("Initializing " << name() << "...");


  //if (m_useGRL) ATH_CHECK( m_grl.retrieve() );
  ATH_CHECK( m_grl.retrieve() );
  ATH_CHECK( m_helper.retrieve() );
  ATH_CHECK( m_trigDecTool.retrieve() );
  ATH_CHECK( m_bcTool.retrieve() );
  ATH_CHECK( m_trigObjMatchTool.retrieve() );

  ATH_CHECK( book( TTree("METTree", "METTree") ) );
  TTree* theTree = tree("METTree");

  for (HLTMET type : {HLTMET::Cell, HLTMET::Topocl, HLTMET::Topocl_PS, HLTMET::Topocl_PUC, HLTMET::mht, HLTMET::L1}) {
    theTree->Branch( (m_helper->getHLTMETName(type)+"_MET").c_str(), &m_trigMet[type]);
    theTree->Branch( (m_helper->getHLTMETName(type)+"_METX").c_str(), &m_trigMetX[type]);
    theTree->Branch( (m_helper->getHLTMETName(type)+"_METY").c_str(), &m_trigMetY[type]);
    theTree->Branch( (m_helper->getHLTMETName(type)+"_MET_wEFMu").c_str(), &m_trigMet_wEFMu[type]);
    theTree->Branch( (m_helper->getHLTMETName(type)+"_METX_wEFMu").c_str(), &m_trigMetX_wEFMu[type]);
    theTree->Branch( (m_helper->getHLTMETName(type)+"_METY_wEFMu").c_str(), &m_trigMetY_wEFMu[type]);
  }
  for (const std::string& trig : m_triggers) {
    theTree->Branch(trig.c_str(), &triggerBits[trig]);
  }
  for (const std::string& signal : m_signals) {
    theTree->Branch(signal.c_str(), &signalBits[signal] );
    caccs.emplace(signal, signal);
  }
  theTree->Branch( "eventNumber", &eventNumber );
  theTree->Branch( "lumiBlock", &lumiBlock );
  theTree->Branch( "runNumber", &runNumber );
  theTree->Branch( "passGRL", &passGRL );
  theTree->Branch( "passTSTCleaning", &passTSTCleaning );
  theTree->Branch( "OfflineMET", &m_met );
  theTree->Branch( "OfflineMETX", &m_metX );
  theTree->Branch( "OfflineMETY", &m_metY );
  theTree->Branch( "OfflineSumEt", &m_sumEt );
  theTree->Branch( "OfflineMETNoTST", &m_metNoTST );
  theTree->Branch( "OfflineMETXNoTST", &m_metXNoTST );
  theTree->Branch( "OfflineMETYNoTST", &m_metYNoTST );
  theTree->Branch( "OfflineSumEtNoTST", &m_sumEtNoTST );
  theTree->Branch( "OfflineMETCST", &m_metCST );
  theTree->Branch( "OfflineMETXCST", &m_metXCST );
  theTree->Branch( "OfflineMETYCST", &m_metYCST );
  theTree->Branch( "OfflineSumEtCST", &m_sumEtCST );
  theTree->Branch( "jetPt", &jetPt );
  theTree->Branch( "jetEta", &jetEta );
  theTree->Branch( "jetPhi", &jetPhi );
  theTree->Branch( "elePt", &elePt );
  theTree->Branch( "eleEta", &eleEta );
  theTree->Branch( "elePhi", &elePhi );
  theTree->Branch( "muPt", &muPt );
  theTree->Branch( "muEta", &muEta );
  theTree->Branch( "muPhi", &muPhi );
  theTree->Branch( "nPrimaryVertices", &nPrimaryVertices );
  theTree->Branch( "outOfTimePileup", &outOfTimePileup );
  theTree->Branch( "inTimePileup", &inTimePileup );
  theTree->Branch( "bcid", &bcid );
  theTree->Branch( "distanceFromFront", &distanceFromFront );
  theTree->Branch( "distanceFromTail", &distanceFromTail );

  std::string HLTName = "HLT_xAOD__CaloClusterContainer_";
  for (const std::string& contName : clusterContainerNames) {
    std::string title = contName;
    if (title.substr(0, HLTName.size() ) == HLTName) title = title.substr(HLTName.size() );
    theTree->Branch( (title+"_clusterPt").c_str(), &clusterPt[contName]);
    theTree->Branch( (title+"_clusterEta").c_str(), &clusterEta[contName]);
    theTree->Branch( (title+"_clusterPhi").c_str(), &clusterPhi[contName]);
    theTree->Branch( (title+"_clusterMass").c_str(), &clusterMass[contName]);
    theTree->Branch( (title+"_clusterMet").c_str(), &clusterMet[contName]);
    ATH_CHECK( book( TH1F( (title+"_clusterMetDiff").c_str(), (title+";MET difference [GeV]").c_str(), 200, -100, 100) ) );
  }

  HLTName = "HLT_xAOD__JetContainer_";
  for (const std::string& contName : trigJetContainerNames) {
    std::string title = contName;
    if (title.substr(0, HLTName.size() ) == HLTName) title = title.substr(HLTName.size() );
    theTree->Branch( (title+"_trigJetPt").c_str(), &trigJetPt[contName]);
    theTree->Branch( (title+"_trigJetEta").c_str(), &trigJetEta[contName]);
    theTree->Branch( (title+"_trigJetPhi").c_str(), &trigJetPhi[contName]);
    theTree->Branch( (title+"_trigJetMass").c_str(), &trigJetMass[contName]);
    theTree->Branch( (title+"_trigJetMet").c_str(), &trigJetMet[contName]);
    theTree->Branch( (title+"_trigMatchedType").c_str(), &trigJetMatchedType[contName]);
    ATH_CHECK( book( TH1F( (title+"_trigJetMetDiff").c_str(), (title+";MET difference [GeV]").c_str(), 200, -100, 100) ) );
  }


  

  return StatusCode::SUCCESS;
}

StatusCode OutputMaker_Tree::finalize() {
  ATH_MSG_INFO ("Finalizing " << name() << "...");
  //
  //Things that happen once at the end of the event loop go here
  //


  return StatusCode::SUCCESS;
}

StatusCode OutputMaker_Tree::execute() {  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  setFilterPassed(false); //optional: start with algorithm not passed


  //
  //Your main analysis code goes here
  //If you will use this algorithm to perform event skimming, you
  //should ensure the setFilterPassed method is called
  //If never called, the algorithm is assumed to have 'passed' by default
  //


  //HERE IS AN EXAMPLE
  const xAOD::EventInfo* evtInfo = 0;
  CHECK( evtStore()->retrieve( evtInfo, "EventInfo" ) );
  inTimePileup = evtInfo->actualInteractionsPerCrossing();
  outOfTimePileup = evtInfo->averageInteractionsPerCrossing();
  eventNumber = evtInfo->eventNumber();
  runNumber = evtInfo->runNumber();
  lumiBlock = evtInfo->lumiBlock();

  bcid = evtInfo->bcid();
  distanceFromFront = m_bcTool->distanceFromFront(bcid, Trig::IBunchCrossingTool::BunchDistanceType::FilledBunches);
  distanceFromTail = m_bcTool->distanceFromTail(bcid, Trig::IBunchCrossingTool::BunchDistanceType::FilledBunches);

  if (m_useGRL && !m_grl->passRunLB(*evtInfo) ) return StatusCode::SUCCESS;
  passGRL = m_grl->passRunLB(*evtInfo);

  for (auto& sigName : m_signals) {
    signalBits.at(sigName) = caccs.at(sigName)(*evtInfo);
  }
  for (auto& pair : triggerBits) {
    pair.second = m_trigDecTool->isPassed(pair.first);
  }
  //for (HLTMET type : {HLTMET::Cell, HLTMET::Topocl, HLTMET::Topocl_PS, HLTMET::Topocl_PUC, HLTMET::mht, HLTMET::L1}) {
  for (HLTMET type : {HLTMET::Cell, HLTMET::Topocl, HLTMET::Topocl_PS, HLTMET::Topocl_PUC, HLTMET::mht}) {
    const xAOD::TrigMissingETContainer* trigCont;
    ATH_CHECK( evtStore()->retrieve(trigCont, m_helper->getHLTMETContainerName(type) ) );
    m_trigMetX.at(type) = trigCont->front()->ex() * 0.001;
    m_trigMetY.at(type) = trigCont->front()->ey() * 0.001;
    m_trigMet.at(type) = sqrt(m_trigMetX.at(type)*m_trigMetX.at(type) + m_trigMetY.at(type)*m_trigMetY.at(type) );
    //ATH_CHECK( m_helper->getMET(type, m_trigMet.at(type) ) );
    //ATH_CHECK( m_helper->getMETX(type, m_trigMetX.at(type) ) );
    //ATH_CHECK( m_helper->getMETY(type, m_trigMetY.at(type) ) );
    //m_trigMet.at(type) * GeV;
    //m_trigMetX.at(type) * GeV;
    //m_trigMetY.at(type) * GeV;
  }
  m_trigMetX_wEFMu = m_trigMetX;
  m_trigMetY_wEFMu = m_trigMetY;
  const xAOD::EnergySumRoI* l1Cont;
  ATH_CHECK( evtStore()->retrieve(l1Cont, "LVL1EnergySumRoI") );
  m_trigMetX.at(HLTMET::L1) = l1Cont->exMiss() * 0.001;
  m_trigMetY.at(HLTMET::L1) = l1Cont->eyMiss() * 0.001;
  m_trigMet.at(HLTMET::L1) = sqrt(m_trigMetX.at(HLTMET::L1)*m_trigMetX.at(HLTMET::L1)+m_trigMetY.at(HLTMET::L1)*m_trigMetY.at(HLTMET::L1) );
  jetPt.clear();
  jetEta.clear();
  jetPhi.clear();
  elePt.clear();
  eleEta.clear();
  elePhi.clear();
  muPt.clear();
  muEta.clear();
  muPhi.clear();
  std::vector<const xAOD::IParticle*> recoObjects;
  const xAOD::JetContainer* jets(0);
  ATH_CHECK( evtStore()->retrieve(jets, m_sgPrefix + "Jets") );
  static SG::AuxElement::ConstAccessor<char> cacc_passOR(m_decorator+":passOR");
  static SG::AuxElement::ConstAccessor<char> cacc_signal(m_decorator);
  static SG::AuxElement::ConstAccessor<char> cacc_passClearning("passCleaning");
  static SG::AuxElement::ConstAccessor<char> cacc_pileupJet("pileupJet");
  for (const xAOD::Jet* ijet : *jets) {
    if (cacc_passOR(*ijet) ) {
      recoObjects.push_back(ijet);
      if (!cacc_passClearning(*ijet) || cacc_pileupJet(*ijet) ) continue;
      jetPt.push_back(ijet->pt() * GeV);
      jetEta.push_back(ijet->eta() );
      jetPhi.push_back(ijet->phi() );
    }
  }
  const xAOD::ElectronContainer* electrons(0);
  ATH_CHECK( evtStore()->retrieve(electrons, m_sgPrefix + "Electrons") );
  for (const xAOD::Electron* iele : *electrons) { 
    if (cacc_passOR(*iele) ) {
      recoObjects.push_back(iele);
      elePt.push_back(iele->pt() * GeV);
      eleEta.push_back(iele->eta() );
      elePhi.push_back(iele->phi() );
    }
  }
  const xAOD::MuonContainer* muons(0);
  ATH_CHECK( evtStore()->retrieve(muons, m_sgPrefix + "Muons") );
  for (const xAOD::Muon* imu : *muons) {
    if (cacc_passOR(*imu) ) {
      recoObjects.push_back(imu);
      muPt.push_back(imu->pt() * GeV);
      muEta.push_back(imu->eta() );
      muPhi.push_back(imu->phi() );
    }
  }
  const xAOD::PhotonContainer* photons(0);
  ATH_CHECK( evtStore()->retrieve(photons, m_sgPrefix + "Photons") );
  for (const xAOD::Photon* iphoton : *photons) {
    if (cacc_passOR(*iphoton) ) {
      recoObjects.push_back(iphoton);
    }
  }
  
  const xAOD::TauJetContainer* taus(0);
  ATH_CHECK( evtStore()->retrieve(taus, m_sgPrefix + "Taus") );
  for (const xAOD::TauJet* itau : *taus) {
    if (cacc_passOR(*itau) ) {
      recoObjects.push_back(itau);
    }
  }

  const xAOD::CaloClusterContainer* clusters(0);
  std::string HLTName = "HLT_xAOD__CaloClusterContainer_";
  for (const std::string& contName : clusterContainerNames) {
    ATH_CHECK( evtStore()->retrieve(clusters, contName) );
    std::string title = contName;
    if (title.substr(0, HLTName.size() ) == HLTName) title = title.substr(HLTName.size() );
    clusterPt.at(contName).clear();
    clusterEta.at(contName).clear();
    clusterPhi.at(contName).clear();
    clusterMass.at(contName).clear();
    float mpx = 0;
    float mpy = 0;
    for (const xAOD::CaloCluster* iclus : *clusters) {
      clusterPt.at(contName).push_back(iclus->pt() * GeV);
      clusterEta.at(contName).push_back(iclus->eta() );
      clusterPhi.at(contName).push_back(iclus->phi() );
      clusterMass.at(contName).push_back(iclus->m() * GeV);
      mpx += iclus->pt() * GeV * cos(iclus->phi() );
      mpy += iclus->pt() * GeV * sin(iclus->phi() );
    }
    clusterMet.at(contName) = sqrt(mpx*mpx + mpy*mpy);
    TH1* histo = hist(title + "_clusterMetDiff");
    if (!histo) return StatusCode::FAILURE;
    histo->Fill(clusterMet.at(contName) - m_trigMet.at(HLTMET::Topocl) );
  }
  const xAOD::JetContainer* constTrigJets(0);
  HLTName = "HLT_xAOD__JetContainer_";
  for (const std::string& contName : trigJetContainerNames) {
    ATH_CHECK( evtStore()->retrieve(constTrigJets, contName) );
    auto trigJetsSC = shallowCopyContainer(*constTrigJets);
    xAOD::JetContainer* trigJets = trigJetsSC.first;
    ATH_MSG_DEBUG( contName << ": " << trigJets->size() );
    if (trigJets->size() == 0) continue; // Don't want to do anything if there are no trigger objects
    // In fact - if there are no trigger objects the containers just might never have been filled... Check? TODO
    std::string title = contName;
    if (title.substr(0, HLTName.size() ) == HLTName) title = title.substr(HLTName.size() );
    trigJetPt.at(contName).clear();
    trigJetEta.at(contName).clear();
    trigJetPhi.at(contName).clear();
    trigJetMass.at(contName).clear();
    trigJetMatchedType.at(contName).clear();
    ATH_CHECK( m_trigObjMatchTool->matchObjects(recoObjects, trigJets) );
    float mpx = 0;
    float mpy = 0;
    for (const xAOD::Jet* iclus : *trigJets) {
      trigJetPt.at(contName).push_back(iclus->pt() * GeV);
      trigJetEta.at(contName).push_back(iclus->eta() );
      trigJetPhi.at(contName).push_back(iclus->phi() );
      trigJetMass.at(contName).push_back(iclus->m() * GeV);
      trigJetMatchedType.at(contName).push_back(getMatchedParticleType(iclus) );
      mpx += iclus->pt() * GeV * cos(iclus->phi() );
      mpy += iclus->pt() * GeV * sin(iclus->phi() );
    }
    trigJetMet.at(contName) = sqrt(mpx*mpx + mpy*mpy);
    TH1* histo = hist(title + "_trigJetMetDiff");
    if (!histo) return StatusCode::FAILURE;
    histo->Fill(trigJetMet.at(contName) - m_trigMet.at(HLTMET::mht) );
  }
  const xAOD::MuonContainer* trigMuons(0);
  //ATH_CHECK( evtStore()->retrieve(trigMuons, "HLT_xAOD__MuonContainer_MuonEFInfo_FullScan") );
  ATH_CHECK( evtStore()->retrieve(trigMuons, "HLT_xAOD__MuonContainer_eMuonEFInfo") );
  for (const xAOD::Muon* imu : *trigMuons) {
    if (imu->pt() * GeV < 8) continue; // 8GeV cut on included muons
    float ptx = imu->pt() * GeV * cos(imu->phi() );
    float pty = imu->pt() * GeV * sin(imu->phi() );
    for (HLTMET type : {HLTMET::Cell, HLTMET::Topocl, HLTMET::Topocl_PS, HLTMET::Topocl_PUC, HLTMET::mht}) {
      m_trigMetX_wEFMu.at(type) += ptx;
      m_trigMetY_wEFMu.at(type) += pty;
    }
  }
  for (HLTMET type : {HLTMET::Cell, HLTMET::Topocl, HLTMET::Topocl_PS, HLTMET::Topocl_PUC, HLTMET::mht}) {
    m_trigMet_wEFMu.at(type) = sqrt(m_trigMetX_wEFMu.at(type) * m_trigMetX_wEFMu.at(type) + m_trigMetY_wEFMu.at(type) * m_trigMetY_wEFMu.at(type) );
  }
  const xAOD::VertexContainer* vertices;
  ATH_CHECK( evtStore()->retrieve(vertices, "PrimaryVertices") );
  nPrimaryVertices = vertices->size();

  const xAOD::MissingETContainer* metCont;
  ATH_CHECK( evtStore()->retrieve(metCont, m_sgPrefix + "EtMissNoMu") );

  auto metItr = metCont->find("FinalTrk");
  if (metItr == metCont->end() ) {
    ATH_MSG_ERROR( "FinalTrk not present in container " << m_sgPrefix << "EtMissNoMu" );
    return StatusCode::FAILURE;
  }
  m_met = (*metItr)->met() * GeV;
  m_metX = (*metItr)->mpx() * GeV;
  m_metY = (*metItr)->mpy() * GeV;
  m_sumEt = (*metItr)->sumet() * GeV;

  float offMetPhi = (*metItr)->phi();

  metItr = metCont->find("PVSoftTrk");
  if (metItr == metCont->end() ) {
    ATH_MSG_ERROR( "PVSoftTrk not present in container " << m_sgPrefix << "EtMissNoMu" );
    return StatusCode::FAILURE;
  }
  m_metXNoTST = m_metX - (*metItr)->mpx() * GeV;
  m_metYNoTST = m_metY - (*metItr)->mpy() * GeV;
  m_sumEtNoTST = m_sumEt - (*metItr)->sumet() * GeV;
  m_metNoTST = sqrt(m_metXNoTST*m_metXNoTST + m_metYNoTST*m_metYNoTST);

  float TSTMet = (*metItr)->met() * GeV;
  float TSTPhi = (*metItr)->phi();

  passTSTCleaning = (TSTMet < 1000) || (m_met > 0 && TSTMet/m_met < 0.4) || (fabs(TVector2::Phi_mpi_pi(TSTPhi - offMetPhi) ) > 0.8);

  auto metCSTItr = metCont->find("FinalClus");
  if (metCSTItr == metCont->end() ) {
    ATH_MSG_ERROR( "FinalClus not present in container " << m_sgPrefix << "EtMissNoMu" );
    return StatusCode::FAILURE;
  }
  m_metCST = (*metCSTItr)->met() * GeV;
  m_metXCST = (*metCSTItr)->mpx() * GeV;
  m_metYCST = (*metCSTItr)->mpy() * GeV;
  m_sumEtCST = (*metCSTItr)->sumet() * GeV;

  //ATH_MSG_INFO("eventNumber=" << evtInfo->eventNumber() );
  tree("METTree")->Fill();


  setFilterPassed(true); //if got here, assume that means algorithm passed
  return StatusCode::SUCCESS;
}

StatusCode OutputMaker_Tree::beginInputFile() { 
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


