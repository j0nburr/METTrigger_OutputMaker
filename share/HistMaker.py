#Skeleton joboption for a simple analysis job

print "Starting joboptions"

output = vars().get("output", "output")
theApp.EvtMax=vars().get("nEvents", -1)                                         #says how many events to run over. Set to -1 for all events

import AthenaPoolCnvSvc.ReadAthenaPool                   #sets up reading of POOL files (e.g. xAODs)
svcMgr.EventSelector.InputCollections=[vars().get("input", "/data/atlas/atlasdata3/burr/xAOD/testFiles/data15_13TeV.00284285.physics_Main.merge.AOD.r7562_p2521/AOD.07687825._003463.pool.root.1")]   #insert your list of input files here

# Check if it is data16 or data15
useGRL = ("data15_13TeV" in svcMgr.EventSelector.InputCollections[0])

from PathResolver import PathResolver
if useGRL: ToolSvc += CfgMgr.GoodRunsListSelectionTool("GoodRunsListSelectionTool", PassThrough = False, GoodRunsListVec = [PathResolver.FindCalibFile("data15_13TeV.periodAllYear_DetStatus-v75-repro20-01_DQDefects-00-02-02_PHYS_StandardGRL_All_Good_25ns.xml")])

# algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
algseq = CfgMgr.AthSequencer("AthAlgSeq")                #gets the main AthSequencer
outputLevel = vars().get("msgLevel", INFO)
algseq += CfgMgr.GetMETTriggerObjects("Getter", IsData = vars().get("isData", True), OutputModifier = "Cal", ConfigFiles = ["WZCommon2016.json", "defaultMetConfig.json", "LooseLeptons.json"], OutputLevel = outputLevel)                                 #adds an instance of your alg to it
selector = CfgMgr.WZCommonEventSelector("WZCommonEventSelector", ConfigFile = "WZCommon2016.json", StoreGatePrefix = "Cal")
ToolSvc += selector
algseq += CfgMgr.ApplySelector("WZCommonSelector", Selector = selector, Signals = ["Wenu", "Wmunu", "Zmumu"], SetFilter = False, RootStreamName = "CUTFLOW", OutputLevel = outputLevel)
objectCounter = CfgMgr.ObjectCounter("LeptonCounter", ConfigFile = "LooseLeptons.json", StoreGatePrefix = "Cal", OutputLevel = outputLevel)
ToolSvc += objectCounter
algseq += CfgMgr.ApplySelector("LeptonSelector", Selector = objectCounter, Signals = ["electron", "muon"], SetFilter = False, RootStreamName = "CUTFLOW", OutputLevel = outputLevel)
for signal in ["Wenu", "Wmunu", "Zmumu"]:
  algseq += CfgMgr.OutputMaker_Hists(signal+"HistMakerCST", Signals = [signal], UseGRL = useGRL, StoreGatePrefix = "Cal", UseCST = True, RootStreamName = "HIST", DirName = "CST/"+signal, OutputLevel = outputLevel)
  algseq += CfgMgr.MultiJetTurnOnMakerAlg(signal+"MJMaker", Signals = [signal], UseGRL = useGRL, StoreGatePrefix = "Cal", Decorator = "WZCommon", RootStreamName = "HIST", DirName = "MJets/"+signal, OutputLevel = outputLevel)
  algseq += CfgMgr.OutputMaker_Hists(signal+"HistMakerNoTST", Signals = [signal], UseGRL = useGRL, StoreGatePrefix = "Cal", RemoveSoftTerm = True, RootStreamName = "HIST", DirName = "NoTST/"+signal, OutputLevel = outputLevel)
for signal in ["electron", "muon"]:
  algseq += CfgMgr.OutputMaker_Hists(signal+"HistMakerCST", Signals = [signal], UseGRL = useGRL, StoreGatePrefix = "Cal", UseCST = True, RootStreamName = "HIST", DirName = "CST/"+signal, OutputLevel = outputLevel)
  algseq += CfgMgr.OutputMaker_Hists(signal+"HistMakerNoTST", Signals = [signal], UseGRL = useGRL, StoreGatePrefix = "Cal", RemoveSoftTerm = True, RootStreamName = "HIST", DirName = "NoTST/"+signal, OutputLevel = outputLevel)
  algseq += CfgMgr.MultiJetTurnOnMakerAlg(signal+"MJMaker", Signals = [signal], UseGRL = useGRL, StoreGatePrefix = "Cal", Decorator = "LooseLepton", RootStreamName = "HIST", DirName = "MJets/"+signal, OutputLevel = outputLevel)
algseq += CfgMgr.OutputMaker_Hists("leptonHistMakerCST", Signals = ["electron", "muon"], UseGRL = useGRL, StoreGatePrefix = "Cal", UseCST = True, RootStreamName = "HIST", DirName = "CST/lepton", OutputLevel = outputLevel)
algseq += CfgMgr.OutputMaker_Hists("leptonHistMakerNoTST", Signals = ["electron", "muon"], UseGRL = useGRL, StoreGatePrefix = "Cal", RemoveSoftTerm = True, RootStreamName = "HIST", DirName = "NoTST/lepton", OutputLevel = outputLevel)
algseq += CfgMgr.MultiJetTurnOnMakerAlg("leptonMJMaker", Signals = ["electron", "muon"], UseGRL = useGRL, StoreGatePrefix = "Cal", Decorator = "LooseLepton", RootStreamName = "HIST", DirName = "MJets/lepton", OutputLevel = outputLevel)
algseq += CfgMgr.OutputMaker_Hists("backgroundHistMakerCST", Signals = [], UseGRL = useGRL, StoreGatePrefix = "Cal", UseCST = True, RootStreamName = "HIST", DirName = "CST/background", OutputLevel = outputLevel)
algseq += CfgMgr.OutputMaker_Hists("backgroundHistMakerNoTST", Signals = [], UseGRL = useGRL, StoreGatePrefix = "Cal", RemoveSoftTerm = True, RootStreamName = "HIST", DirName = "NoTST/background", OutputLevel = outputLevel)
algseq += CfgMgr.MultiJetTurnOnMakerAlg("backgroundMJMaker", Signals = [], UseGRL = useGRL, StoreGatePrefix = "Cal", Decorator = "LooseLepton", RootStreamName = "HIST", DirName = "MJets/background", OutputLevel = outputLevel)


##--------------------------------------------------------------------
## This section shows up to set up a HistSvc output stream for outputing histograms and trees
## See https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/AthAnalysisBase#How_to_output_trees_and_histogra for more details and examples

if not hasattr(svcMgr, 'THistSvc'): svcMgr += CfgMgr.THistSvc() #only add the histogram service if not already present (will be the case in this jobo)
svcMgr.THistSvc.Output += ["CUTFLOW DATAFILE='"+output+"_cutflows.root' OPT='RECREATE'"] #add an output root file stream
svcMgr.THistSvc.Output += ["HIST DATAFILE='"+output+"_hists.root' OPT='RECREATE'"] #add an output root file stream


##--------------------------------------------------------------------


include("AthAnalysisBaseComps/SuppressLogging.py")              #Optional include to suppress as much athena output as possible. Keep at bottom of joboptions so that it doesn't suppress the logging of the things you have configured above
MessageSvc.Format = "% F%50W%S%7W%R%T %0W%M"
MessageSvc.defaultLimit = 9999999999
