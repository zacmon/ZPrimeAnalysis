#ifndef ZPRIMEANALYSIS_LIB_TRUTHSYSTEM
#define ZPRIMEANALYSIS_LIB_TRUTHSYSTEM

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

#include <vector>

#include <TClonesArray.h>
#include <TLorentzVector.h>


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
    std::vector<GenParticle*> getZMuons();
    
    void getNextGeneration(GenParticle* mother, std::vector<GenParticle*> *unstableDaughters, std::vector<GenParticle*> *stableDaughters);
    void organizeVector(std::vector<GenParticle*> *vectorPair);
    std::vector<GenParticle*> getDaughters(GenParticle* mother);
    std::vector<GenParticle*> getDaughters(std::vector<GenParticle*> mothers);
    std::vector<GenParticle*> getChargedStableDaughters(std::vector<GenParticle*> daughters);
    
    TLorentzVector getMomentum(std::vector<GenParticle*> particles);

    void sortParticlesByPt(std::vector<GenParticle*>* particles);
};

#endif
