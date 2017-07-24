#ifndef ZPRIMEANALYSIS_LIB_TRUTHSYSTEM
#define ZPRIMEANALYSIS_LIB_TRUTHSYSTEM

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

#include <vector>

#include <TClonesArray.h>

class TruthSystem {
  public:
    std::vector<GenParticle*> genParticles;

    TruthSystem(TClonesArray* branchParticle);
    TruthSystem(TClonesArray* branchParticle, bool print);
    ~TruthSystem();

    void printParticleHeader();
    void printParticleInfo(GenParticle* particle, Int_t index);
    
    bool isEmptyBranch(TClonesArray* branch);
    bool isZBoson(GenParticle* particle);
    bool isZPrimeBoson(GenParticle* particle);
    bool isCharged(GenParticle* particle);
    bool isStable(GenParticle* particle);

    std::vector<GenParticle*> getZPrimeQuarks();
    
    std::vector<GenParticle*> getNextGeneration(GenParticle* mother);
};

#endif
