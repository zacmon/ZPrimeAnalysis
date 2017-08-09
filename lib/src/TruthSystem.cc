#include "../interface/TruthSystem.h"

#include <cmath>

TruthSystem::TruthSystem(TClonesArray* branchParticle) {
    if (!isEmptyBranch(branchParticle)) {
	for (int i = 0; i < branchParticle->GetEntriesFast(); ++i) {
	    GenParticle* genParticle = (GenParticle*) branchParticle->At(i);
	    genParticles.push_back(genParticle);
	}
    }
}

TruthSystem::TruthSystem(TClonesArray* branchParticle, bool print) {
    if (!isEmptyBranch(branchParticle)) {
	if (print) {
	    printParticleHeader();
	}
	for (int i = 0; i < branchParticle->GetEntriesFast(); ++i) {
            GenParticle* genParticle = (GenParticle*) branchParticle->At(i);
	    if (print) {
		printParticleInfo(genParticle, i);
		genParticles.push_back(genParticle);
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
           index, particle->PID, particle->Status, particle->IsPU, particle->M1, particle->M2,
           particle->D1, particle->D2, particle->Charge);
    printf("%9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %9.2f %7.2f %7.2f %7.2f %7.2f %7.2f",
	   particle->Mass, particle->E,particle->Px, particle->Py, particle->Pz,
	   particle->P, particle->PT, particle->Eta, particle->Phi,
	   particle->Rapidity, particle -> CtgTheta, particle->D0, particle->DZ,
	   particle->T, particle->X, particle->Y, particle->Z);
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
    return std::abs(particle->PID) == 23;
}

bool TruthSystem::isZPrimeBoson(GenParticle* particle) {
    return std::abs(particle->PID) == 101;
}

bool TruthSystem::isCharged(GenParticle* particle) {
    return particle->Charge != 0;
}

bool TruthSystem::isStable(GenParticle* particle) {
    return particle->Status == 1;
}

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

std::vector<GenParticle*> TruthSystem::getZMuons() {
    std::vector<GenParticle*> muons;
    int zIndex = 1e6;
    for (unsigned int i = 0; i < genParticles.size(); ++i) {
	if (isZBoson(genParticles[i])) {
	    zIndex = i;
	}
	if (zIndex != 1e6 && genParticles[i]->M1 == zIndex && !isZBoson(genParticles[i])) {
	    muons.push_back(genParticles[i]);
	}
	if (muons.size() == 2) {
	    return muons;
	}
    }
    return {};
}

void TruthSystem::getNextGeneration(GenParticle* mother, std::vector<GenParticle*> *unstableDaughters, std::vector<GenParticle*> *stableDaughters) {
    for (int i = mother->D1; i <= mother->D2; ++i) {
	if (genParticles[i]->M1 == genParticles[mother->D1]->M1) {
	    if (isStable(genParticles[i])) {
		stableDaughters->push_back(genParticles[i]);
	    }
	    else {
		unstableDaughters->push_back(genParticles[i]);
	    }
	}
    }
}

void TruthSystem::organizeVector(std::vector<GenParticle*> *particles) {
    unordered_set<GenParticle*> setParticles;
    for (auto &&particle : *particles) {
	setParticles.insert(particle);
    }
    particles->assign(setParticles.begin(), setParticles.end());
}

//  TODO
//  PYTHIA6 IMPLEMENTATION
std::vector<GenParticle*> TruthSystem::getDaughters(GenParticle* mother) {
    if (isStable(mother)) {
	return {mother};
    }
    std::vector<GenParticle*> unstableDaughters = {mother};
    std::vector<GenParticle*> stableDaughters;    
    while (unstableDaughters.size() != 0) {
	getNextGeneration(unstableDaughters[0], &unstableDaughters, &stableDaughters);
	unstableDaughters.erase(unstableDaughters.begin());
    }
    organizeVector(&stableDaughters);
    return stableDaughters;
}

std::vector<GenParticle*> TruthSystem::getDaughters(std::vector<GenParticle*> mothers) {
    std::vector<GenParticle*> unstableDaughters;
    std::vector<GenParticle*> stableDaughters;
    for (auto &&mother : mothers) {
	if (isStable(mother)) {
	    stableDaughters.push_back(mother);
	}
	else {
	    unstableDaughters.push_back(mother);
	}
    }
    while (unstableDaughters.size() != 0) {
        getNextGeneration(unstableDaughters[0], &unstableDaughters, &stableDaughters);
        unstableDaughters.erase(unstableDaughters.begin());
    }
    organizeVector(&stableDaughters);
    return stableDaughters;
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

TLorentzVector TruthSystem::getMomentum(std::vector<GenParticle*> particles) {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&particle : particles) {
	momentum += particle->P4();
    }
    return momentum;
}

void TruthSystem::sortParticlesByPt(std::vector<GenParticle*>* particles) {
    std::sort(particles->begin(), particles->end(), [](GenParticle* a, GenParticle* b) {return a->P4().Pt() > b->P4().Pt();});
}
