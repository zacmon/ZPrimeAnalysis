#include "../interface/JetSystem.h"
#include "ISRTagger.cc"

#include <cmath>

#include <TLorentzVector.h>

JetSystem::JetSystem() {
}

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

double JetSystem::getDistanceFromJetAxis(TLorentzVector pJet, TLorentzVector pConstituent) {
    double dPhiSquared = std::pow(pJet.DeltaPhi(pConstituent), 2);
    double dRapiditySquared = std::pow(pJet.Rapidity() - pConstituent.Rapidity(), 2);
    return std::sqrt(dPhiSquared + dRapiditySquared);
}

//  https://arxiv.org/pdf/1106.3076.pdf
double JetSystem::getJetGirth(Jet* jet) {
    double girth = 0;
    TLorentzVector pJet = jet->P4();
    
    for(int j(0); j < jet->Constituents.GetEntriesFast(); ++j) {
	TLorentzVector pConstituent(0.0, 0.0, 0.0, 0.0);
	TObject* object = jet->Constituents.At(j);
        if(object == 0) {
            continue;
        }
        if(object->IsA() == GenParticle::Class()) {
            GenParticle* particle = (GenParticle*) object;
            pConstituent = particle->P4();
	}
        else if(object->IsA() == Track::Class()) {
            Track* track = (Track*) object;
	    pConstituent = track->P4();
        }
        else if(object->IsA() == Tower::Class()) {
            Tower* tower = (Tower*) object;
            pConstituent = tower->P4();
        }
	girth += pConstituent.Pt() / pJet.Pt() * std::abs(getDistanceFromJetAxis(pJet, pConstituent));
    }
    return girth;
}

std::vector<Jet*> JetSystem::getZPrimeMinJets(std::vector<GenParticle*> daughterQuark) {
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

std::vector<Jet*> JetSystem::getZPrimeJets(std::vector<GenParticle*> quarks) {
    double jetRadius = 0.5;
    std::vector<Jet*> matchedJets;
    if (quarks.size() == 0) {
	return {};
    }
    for (auto &&jet : jets) {
	double deltaRQuark1 = jet->P4().DeltaR(quarks[0]->P4());
	double deltaRQuark2 = jet->P4().DeltaR(quarks[1]->P4());
	if (deltaRQuark1 < jetRadius || deltaRQuark2 < jetRadius) {
	    matchedJets.push_back(jet);
	}
    }
    return matchedJets;
}

std::vector<Jet*> JetSystem::getNoZPrimeJets(std::vector<Jet*> jetsZPrime) {
    std::vector<Jet*> noZPrimeJets;
    for (auto &&jet : jets) {
	auto it = find(jetsZPrime.begin(), jetsZPrime.end(), jet);
	if (it != jetsZPrime.end()) {
	    continue;
	}
	noZPrimeJets.push_back(jet);
    }
    return noZPrimeJets;
}

/*
void JetSystem::vetoMuons(std::vector<Muon*> muons) {
    for (auto it(jets.begin()); it != jets.end(); ++it) {
	if (isMuonJet(*it, muons)) {
	    jets.erase(it);
	}
    }
}
*/
int JetSystem::vetoMuons(std::vector<Muon*> muons) {
    int numMuonJets = 0;
    unsigned int i = 0;
    while (i < jets.size()) {
	if (isMuonJet(jets[i], muons)) {
	    numMuonJets++;
            jets.erase(jets.begin() + i);
        }
	else {
	    ++i;
	}
    }
    return numMuonJets;
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
	    ++i;
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
            ++i;
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

double JetSystem::getDeltaEta(std::vector<Jet*> leadingJets) {
    if (leadingJets.size() == 2) {
	return leadingJets[0]->P4().Eta() - leadingJets[1]->P4().Eta();
    }
    return -999;
}

bool JetSystem::areBelowDeltaEta(std::vector<Jet*> leadingJets, double eta) {
    if (leadingJets.size() == 2) {
	return std::abs(getDeltaEta(leadingJets)) < eta;
    }
    return false;
}

double JetSystem::getDeltaPhi(std::vector<Jet*> leadingJets) {
    if (leadingJets.size() == 2) {
	return leadingJets[0]->P4().DeltaPhi(leadingJets[1]->P4());
    }
    return -999;
}

bool JetSystem::areAboveDeltaPhi(std::vector<Jet*> leadingJets, double phi) {
    if (leadingJets.size() == 2) {
	return std::abs(getDeltaPhi(leadingJets)) > phi;
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

TLorentzVector JetSystem::getMomentum() {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&jet : jets) {
        momentum += jet->P4();
    }
    return momentum;
}

TLorentzVector JetSystem::getMomentum(std::vector<Jet*> leadingJets) {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&jet : leadingJets) {
        momentum += jet->P4();
    }
    return momentum;
}

std::vector<Jet*> JetSystem::getLeadingJets() {
    std::vector<Jet*> leadingJets;
    for (unsigned int i(0); i < jets.size(); ++i) {
	leadingJets.push_back(jets[i]);
	if (leadingJets.size() == 2) {
	    break;
	}
    }
    return leadingJets;
}

void JetSystem::removeISR(double dR) {
    ISRTagger* isrTagger = new ISRTagger();
    unsigned int i = 0;
    while (i < jets.size()) {
	if (isrTagger->deltaR(jets[i], jets) > dR) {
	    jets.erase(jets.begin() + i);
	}
	else {
	    ++i;
	}
    }
}

void JetSystem::sortJets(std::vector<Jet*>* inJets) {
    std::sort(inJets->begin(), inJets->end(), [](Jet* a, Jet* b) {return a->P4().Pt() > b->P4().Pt();});
}

double JetSystem::getDeltaRapidity(std::vector<Jet*> inJets) {
    if (inJets.size() == 2) {
        return inJets[0]->P4().Rapidity() - inJets[1]->P4().Rapidity();
    }
    return -999;
}
