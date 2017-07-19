#ifndef ZPRIMEANALYSIS_LIB_ISRTAGGER
#define ZPRIMEANALYSIS_LIB_ISRTAGGER

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#endif

#include <vector>

class ISRTagger {
  public:
    ISRTagger();
    ~ISRTagger();
 
    double pTRatio(Jet* iJet, Jet* jJet);
    double getDeltaEta(Jet* iJet, Jet* jJet);
    double deltaEta(Jet* jet, std::vector<Jet*> jets);
    double getMassRatio(Jet* jet);
    double massRatio(Jet* iJet, Jet* jJet);

};

#endif
