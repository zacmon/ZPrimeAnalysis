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
struct TruthParticle {
GenParticle* particle;
unsigned int index;
};
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

std::vector<TruthSystem::TruthParticle> getQuarks();
std::vector<TruthSystem::TruthParticle> getNextGeneration(TruthSystem::TruthParticle mother);

void organizeVector(std::vector<TruthSystem::TruthParticle> *daughters);
bool areDaughtersStable(std::vector<TruthSystem::TruthParticle> daughters);
std::vector<TruthSystem::TruthParticle> getDaughters(TruthSystem::TruthParticle truthParticle);
std::vector<GenParticle*> getChargedStableDaughters(std::vector<GenParticle*> daughters);

TLorentzVector getMomentum(std::vector<GenParticle*> particles);
};

#endif
