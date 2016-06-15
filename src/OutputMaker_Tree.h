#ifndef OUTPUTMAKER_OUTPUTMAKER_TREE_H
#define OUTPUTMAKER_OUTPUTMAKER_TREE_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "METTriggerTools/IMETTriggerHelper.h"
#include "GoodRunsLists/IGoodRunsListSelectionTool.h"
#include "TrigDecisionInterface/ITrigDecisionTool.h"
#include "TrigAnalysisInterfaces/IBunchCrossingTool.h"
#include <map>
#include <AsgTools/ToolHandle.h>
#include "AthContainers/AuxElement.h"
#include "TriggerObjectMatching/ITrigRecoObjectMatchingTool.h"

class OutputMaker_Tree: public ::AthAnalysisAlgorithm { 
 public: 
  OutputMaker_Tree( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~OutputMaker_Tree(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private: 
  ToolHandle<IMETTriggerHelper> m_helper;
  ToolHandle<IGoodRunsListSelectionTool> m_grl;
  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;
  ToolHandle<Trig::IBunchCrossingTool> m_bcTool;
  ToolHandle<ITrigRecoObjectMatchingTool> m_trigObjMatchTool;
  std::map<HLTMET, float> m_trigMet;
  std::map<HLTMET, float> m_trigMetX;
  std::map<HLTMET, float> m_trigMetY;
  std::map<HLTMET, float> m_trigMet_wEFMu;
  std::map<HLTMET, float> m_trigMetX_wEFMu;
  std::map<HLTMET, float> m_trigMetY_wEFMu;
  float m_met;
  float m_metX;
  float m_metY;
  float m_sumEt;
  float m_metNoTST;
  float m_metXNoTST;
  float m_metYNoTST;
  float m_sumEtNoTST;
  float m_metCST;
  float m_metXCST;
  float m_metYCST;
  float m_sumEtCST;
  std::vector<float> jetPt;
  std::vector<float> jetEta;
  std::vector<float> jetPhi;
  std::vector<float> elePt;
  std::vector<float> eleEta;
  std::vector<float> elePhi;
  std::vector<float> muPt;
  std::vector<float> muEta;
  std::vector<float> muPhi;
  std::map<std::string, Char_t> signalBits;
  std::map<std::string, SG::AuxElement::ConstAccessor<char> > caccs;
  //std::map<SG::AuxElement::ConstAccessor<char>, Char_t> signalBits;
  std::map<std::string, Char_t> triggerBits;

  std::vector<std::string> clusterContainerNames;

  std::map<std::string, std::vector<float>> clusterPt;
  std::map<std::string, std::vector<float>> clusterEta;
  std::map<std::string, std::vector<float>> clusterPhi;
  std::map<std::string, std::vector<float>> clusterMass;
  std::map<std::string, float> clusterMet;
  std::map<std::string, TH1*> clusterMetDiff;

  std::vector<std::string> trigJetContainerNames;

  std::map<std::string, std::vector<float>> trigJetPt;
  std::map<std::string, std::vector<float>> trigJetEta;
  std::map<std::string, std::vector<float>> trigJetPhi;
  std::map<std::string, std::vector<float>> trigJetMass;
  std::map<std::string, std::vector<int>> trigJetMatchedType;
  std::map<std::string, float> trigJetMet;
  std::map<std::string, TH1*> trigJetMetDiff;

  std::vector<float> trigMuonPt;
  std::vector<float> trigMuonEta;
  std::vector<float> trigMuonPhi;

  int runNumber;
  int eventNumber;
  int lumiBlock;
  char passGRL;
  float inTimePileup;
  float outOfTimePileup;
  unsigned int nPrimaryVertices;
  int bcid;
  int distanceFromFront;
  int distanceFromTail;

  Char_t passTSTCleaning;

  std::string m_sgPrefix;
  std::vector<std::string> m_signals;
  std::vector<std::string> m_triggers;
  bool m_useGRL;
  float GeV;
  std::string m_decorator;
}; 

#endif //> !OUTPUTMAKER_OUTPUTMAKER_TREE_H
