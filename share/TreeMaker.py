#Skeleton joboption for a simple analysis job

print "Starting joboptions"

output = vars().get("output", "output")
theApp.EvtMax=vars().get("nEvents", -1)                                         #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
inFileList = vars().get("inputList", "")
if inFileList == "":
  svcMgr.EventSelector.InputCollections=[vars().get("input", "/data/atlas/atlasdata3/burr/xAOD/testFiles/data15_13TeV.00284285.physics_Main.merge.AOD.r7562_p2521/AOD.07687825._003463.pool.root.1")]   #insert your list of input files here
else:
  svcMgr.EventSelector.InputCollections = open(inFileList).read().splitlines()

# Check if we're using data or MC For now skip this - the batch system doesn't like it
#from PyUtils import AthFile
#af = AthFile.fopen(svcMgr.EventSelector.InputCollections[0])
#isMC = "IS_SIMULATION" in af.fileinfos['evt_type']

isMC = False

from AthenaCommon.GlobalFlags import globalflags
globalflags.DataSource = 'geant4' if isMC else 'data'
globalflags.DatabaseInstance = 'CONDBR2'
from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
if isMC: bcTool = BunchCrossingTool( "MC" )
else: bcTool = BunchCrossingTool( "LHC" )

# Check if it is data16 or data15
# useGRL = ("data15_13TeV" in svcMgr.EventSelector.InputCollections[0])
useGRL = vars().get("useGRL", True)

from PathResolver import PathResolver
ToolSvc += CfgMgr.GoodRunsListSelectionTool("GoodRunsListSelectionTool", PassThrough = False, GoodRunsListVec = [PathResolver.FindCalibFile("data15_13TeV.periodAllYear_DetStatus-v75-repro20-01_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml"), PathResolver.FindCalibFile("data16_13TeV.periodAllYear_DetStatus-v77-pro20-03_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml"), PathResolver.FindCalibFile("data15_13TeV.periodAllYear_DetStatus-v75-repro20-01_DQDefects-00-02-02_PHYS_StandardModel_MinimuBias2010.xml")])

ToolSvc += CfgMgr.TrigRecoObjectMatchingTool("TriggerMatchingTool", OutputLevel=DEBUG)

# algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq = CfgMgr.AthSequencer("MyAlgSeq")                #gets the main AthSequencer
outputLevel = vars().get("msgLevel", INFO)
algseq += CfgMgr.GetMETTriggerObjects("Getter", IsData = vars().get("isData", True), OutputModifier = "Cal", ConfigFiles = ["WZCommon2016.json", "defaultMetConfig.json", "LooseLeptons.json", "TriggerMatchedObjects.json"], OutputLevel = outputLevel, FullDebug = True)                                 #adds an instance of your alg to it
# algseq += CfgMgr.GetMETTriggerObjects("Getter", IsData = vars().get("isData", True), OutputModifier = "Cal", ConfigFiles = ["WZCommonLoose2016.json", "defaultMetConfig.json"], OutputLevel = outputLevel, FullDebug = True)                                 #adds an instance of your alg to it
# selector = CfgMgr.WZCommonEventSelector("WZCommonEventSelector", ConfigFile = "WZCommonLoose2016.json", StoreGatePrefix = "Cal")
selector = CfgMgr.WZCommonEventSelector("WZCommonEventSelector", ConfigFile = "WZCommon2016.json", StoreGatePrefix = "Cal", OutputLevel = outputLevel)
ToolSvc += selector
algseq += CfgMgr.ApplySelector("WZCommonSelector", Selector = selector, Signals = ["Wenu", "Wmunu", "Zmumu"], SetFilter = False, RootStreamName = "CUTFLOW", OutputLevel = outputLevel)
objectCounter = CfgMgr.ObjectCounter("LeptonCounter", ConfigFile = "LooseLeptons.json", StoreGatePrefix = "Cal", OutputLevel = outputLevel)
ToolSvc += objectCounter
algseq += CfgMgr.ApplySelector("LeptonSelector", Selector = objectCounter, Signals = ["electron", "muon"], SetFilter = False, RootStreamName = "CUTFLOW", OutputLevel = outputLevel)
triggerList = ["HLT_j400", "HLT_j360", "HLT_j320", "HLT_j260", "HLT_j200", "HLT_j175", "HLT_j150", "HLT_j110", "HLT_j85", "HLT_7j45_L15J50ETA25", "HLT_7j45_L14J15", "HLT_7j45", "HLT_6j50_0eta240_L14J15", "HLT_6j45_0eta240", "HLT_5j70_0eta240", "HLT_5j65_0eta240", "HLT_5j70_L14J15", "HLT_5j70", "HLT_5j80", "HLT_5j60", "HLT_5j55", "HLT_5j45", "HLT_4j85", "HLT_xe90_tc_lcw_wEFMu_L1XE50", "HLT_xe90_tc_lcw_L1XE50", "HLT_xe80_tc_lcw_L1XE50", "HLT_xe100_tc_em_L1XE50", "HLT_xe110_pufit_L1XE50", "HLT_xe110_pueta_L1XE50", "HLT_xe90_mht_L1XE50", "HLT_xe100_L1XE50", "HLT_e24_lhmedium_L1EM20VH", "HLT_e60_lhmedium", "HLT_mu24_imedium", "HLT_mu50"]
algseq += CfgMgr.OutputMaker_Tree("TreeMaker", Signals = ["Pass_Wenu", "Pass_Wmunu", "Pass_Zmumu", "Pass_electron", "Pass_muon"], Triggers = triggerList, Decorator = "TriggerMatchedObjects", UseGRL = useGRL, StoreGatePrefix = "Cal", RootStreamName = "TREE", BCTool = bcTool, OutputLevel = outputLevel)
# algseq += CfgMgr.OutputMaker_Tree("TreeMaker", Signals = ["Pass_Wenu", "Pass_Wmunu", "Pass_Zmumu"], Triggers = triggerList, Decorator = "WZCommonLoose", UseGRL = useGRL, StoreGatePrefix = "Cal", RootStreamName = "TREE", OutputLevel = outputLevel)


masterseq = CfgMgr.AthSequencer("AthAlgSeq")
masterseq += algseq

##--------------------------------------------------------------------
## This section shows up to set up a HistSvc output stream for outputing histograms and trees
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_output_trees_and_histogra for more details and examples

if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc() #only add the histogram service if not already present (will be the case in this jobo)
svcMgr.THistSvc.Output += ["CUTFLOW DATAFILE='"+output+"_cutflows.root' OPT='RECREATE'"] #add an output root file stream
# svcMgr.THistSvc.Output += ["HIST DATAFILE='"+output+"_hists.root' OPT='RECREATE'"] #add an output root file stream
svcMgr.THistSvc.Output += ["TREE DATAFILE='"+output+"_trees.root' OPT='RECREATE'"] #add an output root file stream


##--------------------------------------------------------------------


include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999999
