
#include "GaudiKernel/DeclareFactoryEntries.h"



#include "../OutputMaker_Hists.h"
DECLARE_ALGORITHM_FACTORY( OutputMaker_Hists )


#include "../OutputMaker_Tree.h"
DECLARE_ALGORITHM_FACTORY( OutputMaker_Tree )

#include "../MultiJetTurnOnMakerAlg.h"
DECLARE_ALGORITHM_FACTORY( MultiJetTurnOnMakerAlg )

DECLARE_FACTORY_ENTRIES( OutputMaker ) 
{
  DECLARE_ALGORITHM( MultiJetTurnOnMakerAlg );
  DECLARE_ALGORITHM( OutputMaker_Tree );
  DECLARE_ALGORITHM( OutputMaker_Hists );
}
