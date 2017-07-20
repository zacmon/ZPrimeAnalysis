#include "JetSystem.h"
#include "ISRTagger.cc"

#include <cmath>

#include <TLorentzVector.h>

JetSystem::JetSystem(TClonesArray* branchJet) {
    if (!isEmptyBranch(branchJet)) {
	for (int i(0); i < branchJet->GetEntriesFast(); ++i) {
	    Jet* jet = (Jet*) branchJet->At(i);
	    jets.push_back(jet);
	}
    }
}

JetSystem::JetSystem(TClonesArray* branchJet, std::vector<Muon*> muons) {
    if (!isEmptyBranch(branchJet)) {
        for (int i(0); i < branchJet->GetEntriesFast(); ++i) {
            Jet* jet = (Jet*) branchJet->At(i);
            jets.push_back(jet);
        }
    }
    vetoMuons(muons);
}

JetSystem::~JetSystem() {
}

bool JetSystem::isEmptyBranch(TClonesArray* branch) {
    if (!branch) {
        return 0;
    }
    else {
        return branch->GetEntries() == 0;
    }
}

double JetSystem::getNumJets() {
    return jets.size();
}

bool JetSystem::isMuonJet(Jet* jet, std::vector<Muon*> muons) {
    if(muons.size() > 0) {
	for (auto &&muon : muons) {
	    if (jet->P4().DeltaR(muon->P4()) < 0.5) {
                return true;
            }
        }
    }
    return false;
}

bool JetSystem::isQuarkJet(Jet* jet, std::vector<GenParticle*> daughterQuark) {
    if (daughterQuark.size() > 0) {
	for (auto &&quark : daughterQuark) {
	    if (jet->P4().DeltaR(quark->P4()) < 0.5) {
		return true;
	    }
	}
    }
    return false;
}

TLorentzVector JetSystem::getConstituentsMomentum(Jet* jet) {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);

    for(int j(0); j < jet->Constituents.GetEntriesFast(); ++j) {
        TObject* object = jet->Constituents.At(j);
        if(object == 0) {
            continue;
        }

        if(object->IsA() == GenParticle::Class()) {
            GenParticle* particle = (GenParticle*) object;
            momentum += particle->P4();
        }
        else if(object->IsA() == Track::Class()) {
            Track* track = (Track*) object;
            momentum += track->P4();
        }
        else if(object->IsA() == Tower::Class()) {
            Tower* tower = (Tower*) object;
            momentum += tower->P4();
        }
    }
    return momentum;
}

std::vector<Jet*> JetSystem::getZPrimeJets(std::vector<GenParticle*> daughterQuark) {
    Jet* jetQuark1 = nullptr;
    Jet* jetQuark2 = nullptr;
    double minDeltaR1 = 0.5;
    double minDeltaR2 = 0.5;
    std::vector<Jet*> matchedJets;

    for (unsigned int i(0); i < jets.size(); ++i) {
	double deltaR1 = jets[i]->P4().DeltaR(daughterQuark[0]->P4());
	double deltaR2 = jets[i]->P4().DeltaR(daughterQuark[1]->P4());
	
	if (deltaR1 < deltaR2) {
	    if (deltaR1 < minDeltaR1) {
		minDeltaR1 = deltaR1;
		jetQuark1 = jets[i];
	    }
	}
	else {
	    if (deltaR2 < minDeltaR2) {
		minDeltaR2 = deltaR2;
		jetQuark2 = jets[i];
	    }
	}
    }
    if (jetQuark1) {
        matchedJets.push_back(jetQuark1);
    }
    if (jetQuark2) {
        matchedJets.push_back(jetQuark2);
    }

    return matchedJets;
}

std::vector<std::vector<Jet*>> JetSystem::getSortedJets(std::vector<GenParticle*> daughterQuark) {
    std::vector<std::vector<Jet*>> sortedJets(2);
    if (daughterQuark.size() > 0) {
	sortedJets.push_back(getZPrimeJets(daughterQuark));
	for (unsigned int i(0); i < jets.size(); ++i) {
	    if (sortedJets[0].size() == 2) {
		if (jets[i] == sortedJets[0][0] || jets[i] == sortedJets[0][1]) {
		    continue;
		}
	    }
	    else if (sortedJets[0].size() == 1) {
		if (jets[i] == sortedJets[0][0]) {
		    continue;
		}
	    }		
	    sortedJets[1].push_back(jets[i]);
	}
    }
    return sortedJets;
}

void JetSystem::vetoMuons(std::vector<Muon*> muons) {
    for (auto it(jets.begin()); it != jets.end(); ++it) {
	if (isMuonJet(*it, muons)) {
	    jets.erase(it);
	}
    }
}

bool JetSystem::isWithinEta(Jet* jet, double eta) {
    return std::abs(jet->P4().Eta()) < eta;
}

void JetSystem::cutEta(double eta) {
    unsigned int i = 0;
    while (i < jets.size()) {
	if (!isWithinEta(jets[i], eta)) {
	    jets.erase(jets.begin() + i);
	}
	else {
	    i++;
	}
    }
}

bool JetSystem::isAbovePtThreshold(Jet* jet, double pTThreshold) {
    return jet->P4().Pt() > pTThreshold;
}

void JetSystem::cutPt(double pTThreshold) {
    unsigned int i = 0;
    while (i < jets.size()) {
	if (!isAbovePtThreshold(jets[i], pTThreshold)) {
            jets.erase(jets.begin() + i);
	}
    	else {
            i++;
	}
    }
}

std::vector<Jet*> JetSystem::getJets() {
    return jets;
}

bool JetSystem::areAbovePtThreshold(double pTThreshold, int numJets) {
    int counter = 0;
    for (auto &&jet : jets) {
	if (jet->P4().Pt() > pTThreshold) {
	    counter++;
	}
    }
    return counter >= numJets;
}

bool JetSystem::areBelowDeltaEta(std::vector<Jet*> leadingJets, double eta) {
    if (leadingJets.size() == 2) {
	return std::abs(leadingJets[0]->P4().Eta() - leadingJets[1]->P4().Eta()) < eta;
    }
    return false;
}

bool JetSystem::areAboveDeltaPhi(std::vector<Jet*> leadingJets, double phi) {
    if (leadingJets.size() == 2) {
	return std::abs(leadingJets[0]->P4().Phi() - leadingJets[1]->P4().Phi()) > phi;
    }
    return false;
}

double JetSystem::getMass(std::vector<Jet*> leadingJets) {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&jet : leadingJets) {
	momentum += jet->P4();
    }
    return momentum.M();
}

bool JetSystem::withinMassRange(std::vector<Jet*> leadingJets, double minMass, double maxMass) {
    double massLeadingJets = getMass(leadingJets);
    if (massLeadingJets > minMass && massLeadingJets < maxMass) {
	return true;
    }
    return false;
}

void JetSystem::removeISR() {
    ISRTagger* taggerISR = new ISRTagger();
    unsigned int i = 0;
    while (i < jets.size()) {
        if (taggerISR->deltaEta(jets[i], jets) > 1) {
            jets.erase(jets.begin() + i);
        }
        else {
            i++;
        }
    }
} 

std::vector<Jet*> JetSystem::getLeadingJets() {
    ISRTagger* taggerISR = new ISRTagger();
    
    std::vector<Jet*> leadingJets;
    for (unsigned int i(0); i < jets.size(); ++i) {
	leadingJets.push_back(jets[i]);
	if (leadingJets.size() == 2) {
	    break;
	}
    }
    return leadingJets;
}

bool JetSystem::existsZLeadingJets() {
    if (getNumJets() < 2) {
	return false;
    }
    if (!areAbovePtThreshold(30, 2)) {
	return false;
    }
    std::vector<Jet*> leadingJets = getLeadingJets();
    if (areBelowDeltaEta(leadingJets, 1.75) && areAboveDeltaPhi(leadingJets, 1.6)) {
	if (withinMassRange(leadingJets, 115, 300)) {
	    return true;
	}
    }
}
