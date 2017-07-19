#include <TruthSystem.h>

#include <vector>
#include <iostream>

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

#inclue <TClonesArray.h>

TruthSystem::TruthSystem(TClonesArray* branchParticle) {
    if (!isEmptyBranch(branchParticle)) {
	for (int i(0); i < branchParticle->GetEntriesFast(); ++i) {
	    GenParticle* inGenParticle = (GenParticle*) branchParticle->At(i);
	    genParticles.push_back(inGenParticle);
	}
    }
}

TruthSystem::~TruthSystem() {
}

bool TruthSystem::isEmptyBranch(TClonesArray* branch) {
    if (!branch) {
        return 0;
    }
    else {
        return branch->GetEntries() == 0;
    }
}

bool TruthSystem::isZBoson(GenParticle* particle) {
    return abs(particle->PID) == 23;
}

bool TruthSystem::isZPrimeBoson(GenParticle* particle) {
    return abs(particle->PID) == 101;
}

bool TruthSystem::isCharged(GenParticle* particle) {
    return particle->Charge != 0;
}

bool TruthSystem::isStable(GenParticle* particle) {
    return particle->Status == 1;
}

TLorentzVector TruthSystem::getMomentumGenParticle(std::vector<GenParticle*> genParticles) {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto particle : genParticles) {
        momentum += particle->P4();
    }
    return momentum;
}

void organizeVector(std::vector<GenParticle> *vectorParticles) {
    std::sort(vectorParticles->begin(), vectorParticles->end(), [](TruthParticle a, TruthParticle b) {return a.index < b.index;});
    auto it = std::unique(vectorParticles->begin(), vectorParticles->end(), [](TruthParticle a, TruthParticle b) {return a.index == b.index;});
    vectorDaughters->erase(it, vectorParticles->end());
}

std::vector<GenParticle*> TruthSystem::getStableDaughters(GenParticle* parent) {
    std::vector<GenParticle*> stableDaughters;
    std::vector<GenParticle*> unstableDaughters;

    if (!isStable(parent)) {
	unstableDaughters.push_back(parent);
    }

    while (unstableDaughters.size()) {
	for (auto it(unstableDaughters.begin()); it != unstableDaughters.end(); ++it) {
	    GenParticle* mother = *it;
	    for (int i = mother->D1; i <= mom->D2; ++i) {
		GenParticle* daughter = genParticles[i];
		if (daughter->M1 != genParticles[mother->D1]->M1) {
		    continue;
		}
		if (isStable(daughter)) {
		    stableDaughters.push_back(daughter);
		}
		else {
		    unstableList.push_back(daughter);
		}
	    }
	    unstableDaughters.erase(it);
	}
    }
    organizeVector(&stableDaughters);
    return stableDaughters;
}

std::vector<GenParticle*> TruthSystem::getChargedStableDaughter(GenParticle* parent) {
    std::vector<GenParticle*> chargedStableDaughters;
    std::vector<GenParticle*> stableDaughters = getStableDaughters(parent);
    for (auto daughter : stableDaughters) {
	if (isCharged(daughter)) {
	    chargedStableDaughters.push_back(daughter);
	}
    }
    return chargedStableDaughters;
}
