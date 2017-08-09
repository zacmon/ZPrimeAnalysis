#ifndef ZPRIMEANALYSIS_LIB_ISRTAGGER
#define ZPRIMEANALYSIS_LIB_ISRTAGGER

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#endif

#include <vector>

#include <TLorentzVector.h>

class ISRTagger {
  public:
    ISRTagger();
    ~ISRTagger();
 
    double pTRatio(Jet* iJet, Jet* jJet);

    double getDeltaEta(Jet* iJet, Jet* jJet);
    double deltaEta(Jet* jet, std::vector<Jet*> jets);

    double getDeltaPhi(Jet* iJet, Jet* jJet);
    double deltaPhi(Jet* jet, std::vector<Jet*> jets);

    double getDeltaR(Jet* iJet, Jet* jJet);
    double deltaR(Jet* jet, std::vector<Jet*> jets);
    
    double getMassRatio(Jet* jet);
    double massRatio(Jet* iJet, Jet* jJet);

    int getNumTracks(Jet* jet, std::vector<Track*> tracks, double jetRadius);
};

#endif
