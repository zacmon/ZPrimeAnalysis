#ifndef ZPRIMEANALYSIS_LIB_JETSYSTEM
#define ZPRIMEANALYSIS_LIB_JETSYSTEM

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#endif

#include <vector>
#include <cstdlib>

#include <TClonesArray.h>
#include <TLorentzVector.h>

class JetSystem {
  public:
    std::vector<Jet*> jets;
    
    JetSystem(TClonesArray* branchJet);
    ~JetSystem();
    
    bool isEmptyBranch(TClonesArray* branch);
    double getNumJets();

    bool isMuonJet(Jet* jet, std::vector<Muon*> muons);
    bool isQuarkJet(Jet* jet, std::vector<GenParticle*> quarks);
    
    TLorentzVector getConstituentsMomentum(Jet* jet);
    
    std::vector<Jet*> getZPrimeJets(std::vector<GenParticle*> quarks);
    std::vector<std::vector<Jet*>> getSortedJets(std::vector<GenParticle*> quarks);
    
    void vetoMuons(std::vector<Muon*> muons);

    std::vector<Jet*> getJets();
    std::vector<Jet*> getLeadingJets();
};

#endif
