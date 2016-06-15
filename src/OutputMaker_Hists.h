#ifndef OUTPUTMAKER_OUTPUTMAKER_HISTS_H
#define OUTPUTMAKER_OUTPUTMAKER_HISTS_H 1

#include "AthAnalysisBaseComps/AthAnalysisAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"

#include "METTriggerTools/IMETTriggerHelper.h"
#include "AthContainers/AuxElement.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <TH3.h>

class IGoodRunsListSelectionTool;

class OutputMaker_Hists: public ::AthAnalysisAlgorithm { 
 public: 
  OutputMaker_Hists( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~OutputMaker_Hists(); 

  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();
  
  virtual StatusCode beginInputFile();

 private:
  ToolHandle<IGoodRunsListSelectionTool> m_grl;
  ToolHandle<IMETTriggerHelper> m_helper;
  std::vector<HLTMET> m_trigMets;
  std::vector<HLTMET> m_hltOnlyTrigMets;
  float GeV;
  bool m_useGRL;
  std::map<HLTMET, float> m_lowestUnprescaledCuts;
  std::map<HLTMET, float> m_equalRateCuts;
  std::vector<std::string> m_signals;
  std::vector<SG::AuxElement::ConstAccessor<char>> m_cacc_signals;
  bool m_noSoft;
  bool m_useCST;
  std::string m_sgPrefix;
  std::map<std::string, TH3*> m_th3s;
  std::string m_dirName;
}; 

#endif //> !OUTPUTMAKER_OUTPUTMAKER_HISTS_H
