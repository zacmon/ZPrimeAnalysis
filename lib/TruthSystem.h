#ifndef ZPRIMEANALYSIS_LIB_TRUTHSYSTEM
#define ZPRIMEANALYSIS_LIB_TRUTHSYSTEM

class TruthSystem {
  public:
    std::vector<GenParticle*> genParticles;
    std::vector<GenParticle*> unstableDaughters;
    std::vector<GenParticle*> stableDaughters;
    TruthSystem(TClonesArray* branchParticle);
    ~TruthSystem();

    bool isEmptyBranch(TClonesArray* branch);
    bool isZBoson(GenParticle* particle);
    bool isZPrimeBoson(GenParticle* particle);
    bool isZPrimeBoson(GenParticle* particle);
    bool hasDaughter(GenParticle* particle);
    bool hasDaughters(GenParticle* particle);
    bool isStable(GenParticle* particle);
    bool areDaughtersStable();
    
};

#endif
