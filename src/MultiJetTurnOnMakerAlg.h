#ifndef MULTIJETTURNONMAKER_MULTIJETTURNONMAKERALG_H
#define MULTIJETTURNONMAKER_MULTIJETTURNONMAKERALG_H 1

//#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"

#include "AthenaBaseComps/AthHistogramAlgorithm.h"
//#include "AsgTools/AnaToolHandle.h"
#include "AsgTools/ToolHandle.h"

#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <utility>
#include <tuple>

#include "AthContainers/AuxElement.h"

#include <TH2.h>

namespace Trig {
  class ITrigDecisionTool;
}

class IGoodRunsListSelectionTool;

class MultiJetTurnOnMakerAlg: public ::AthHistogramAlgorithm { 
 public: 
  MultiJetTurnOnMakerAlg( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~MultiJetTurnOnMakerAlg(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private:

  //asg::AnaToolHandle<ST::ISUSYObjDef_xAODTool> m_objTool;
  ToolHandle<Trig::ITrigDecisionTool> m_trigDecTool;
  ToolHandle<IGoodRunsListSelectionTool> m_grl;
  std::vector<std::string> triggerStrings;
  std::unordered_map<std::string, std::vector<float> > m_jetCollections;
  std::vector<std::tuple<std::string, int, std::string> > m_trigNames;
  bool firstEvent;
  //std::vector<std::string> m_refTriggers;
  std::string m_sgPrefix;
  std::vector<std::string> m_signals;
  std::vector<SG::AuxElement::ConstAccessor<char>> m_cacc_signals;
  std::string m_decorator;
  float GeV;
  bool m_useGRL;
  std::map<std::string, TH2*> m_th2s;
  std::string m_dirName;
}; 

#endif //> !MULTIJETTURNONMAKER_MULTIJETTURNONMAKERALG_H
