#include "TruthSystem.h"

TruthSystem::TruthSystem(TClonesArray* branchParticle, int PID) {
    if (!isEmptyBranch(branchParticle)) {
	for (int i(0); i < branchParticle->GetEntriesFast(); ++i) {
	    GenParticle* inGenParticle = (GenParticle*) branchParticle->At(i);
	    genParticles.push_back(inGenParticle);
	}
    }
}

TruthSystem::TruthSystem(TClonesArray* branchParticle, bool print) {
    if (!isEmptyBranch(branchParticle)) {
	if (print) {
	    printParticleHeader();
	}
	for (int i(0); i < branchParticle->GetEntriesFast(); ++i) {
            GenParticle* inGenParticle = (GenParticle*) branchParticle->At(i);
	    if (print) {
		printParticleInfo(inGenParticle, i);
		genParticles.push_back(inGenParticle);
	    }
        }
	if (print) {
	    std::cout << "\n----------------Finished Printing----------------\n\n" << std::endl;
	}
    }
}

TruthSystem::~TruthSystem() {
}

void TruthSystem::printParticleHeader() {
    printf("\n \n %6s %3s %4s %1s %3s %4s %6s %6s %6s",
           "Index", "PID", "Status", "IsPU", "M1", "M2", "D1", "D2", "Charge");
    printf("%6s %7s %9s %10s %8s %10s %9s %8s %10s %9s %12s %6s %7s %6s %7s %7s %7s",
           "Mass", "E", "Px", "Py", "Pz", "P", "PT", "Eta", "Phi", "Rapid", "CtgTheta",
           "D0", "DZ", "T", "X", "Y", "Z");
    std::cout << "\n--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n";
}

void TruthSystem::printParticleInfo(GenParticle* particle, Int_t index) {
    printf("\n %3d %6d %4d %4d %4d %5d %6d %6d %3d",
           index, particle -> PID, particle -> Status, particle -> IsPU, particle -> M1, particle -> M2,
           particle -> D1, particle -> D2, particle -> Charge);
  printf("%9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %7.2f %7.2f %7.2f %7.2f %7.2f",
         particle -> Mass,
         particle -> E,particle -> Px, particle -> Py, particle -> Pz,
         particle -> P, particle -> PT, particle -> Eta, particle -> Phi,
         particle -> Rapidity, particle -> CtgTheta,
         particle -> D0, particle -> DZ,
         particle -> T, particle -> X, particle -> Y, particle -> Z);
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

//  PYTHIA6 IMPLEMENTATION
std::vector<GenParticle*> TruthSystem::getZPrimeQuarks() {
    std::vector<GenParticle*> quarks;
    int zPrimeIndex = 1e6;
    for (unsigned int i = 0; i < genParticles.size(); ++i) {
	if (isZPrimeBoson(genParticles[i])) {
	    zPrimeIndex = i;
	}
	if (zPrimeIndex != 1e6 && genParticles[i]->M1 == zPrimeIndex && !isZPrimeBoson(genParticles[i])) {
	    quarks.push_back(genParticles[i]);
	}
	if (quarks.size() == 2) {
	    return quarks;
	}
    }
    return {};
}

// TODO CHECK CONSERVATION OF MOMENTUM AT EACH STEP
// TODO RETRIEVE ALL MOTHERS SO ORGANIZE VECTOR DOESN'T HAVE TO EXIST
std::vector<GenParticle*> TruthSystem::getNextGeneration(GenParticle* mother) {
    std::vector<GenParticle*> nextGeneration;
    if (!isStable(mother)) {
        for (int j = mother->D1; j <= mother->D2; ++j) {
	    if (genParticles[j]->M1 == genParticles[mother->D1]->M1) {
                nextGeneration.push_back(genParticles[j]);
	    }
        }
    }
    return nextGeneration;
}

void TruthSystem::organizeVector(std::vector<GenParticle*> *daughters) {
    auto it = std::unique(daughters->begin(), daughters->end(), [](GenParticle* a, GenParticle* b) {return a->P4() == b->P4();});
    daughters->erase(it, daughters->end());
}

bool areDaughtersStable(std::vector<GenParticle*> daughters) {
    for (auto daughter : daughters) {
        if (!isStable(daughter)) {
            return false;
	}
    }
    return true;
}

std::vector<GenParticle*> TruthSystem::getDaughters(GenParticle* quark) {
    std::vector<GenParticle*> daughters = {quark};
    int i = 0;

    while (!areDaughtersStable(daughters)) {
        if (!isStable(daughters[i])) {
            std::vector<GenParticle*> nextGeneration = getNextGeneration(vectorDaughters[i]);
            daughters.insert(daughters.end(), nextGeneration.begin(), nextGeneration.end());
            daughters.erase(daughters.begin() + i);
	}
        else {
            i++;
        }
    }
    organizeVector(&daughters);
    return daughters;
}

std::vector<GenParticle*> TruthSystem::getChargedStableDaughters(std::vector<GenParticle*> stableDaughters) {
    std::vector<GenParticle*> chargedStableDaughters;
    for (auto daughter : stableDaughters) {
	if (isCharged(daughter)) {
            chargedStableDaughters.push_back(daughter);
        }
    }
    return chargedStableDaughters;
}
