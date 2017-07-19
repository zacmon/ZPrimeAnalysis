#include "MuonSystem.h"

#include <cstdlib>

#include <TLorentzVector.h>

MuonSystem::MuonSystem() {
}

MuonSystem::MuonSystem(TClonesArray* branchMuon) {
    if (!isEmptyBranch(branchMuon)) {
        for (int i(0); i < branchMuon->GetEntriesFast(); ++i) {
            Muon* muon  = (Muon*) branchMuon->At(i);
            muons.push_back(muon);
        }
    }
}

MuonSystem::~MuonSystem() {
}

bool MuonSystem::isEmptyBranch(TClonesArray* branch) {
    if (!branch) {
	std::cout << "Muon branch does not exist!" << std::endl;
	return 0;
	
    }
    else {
	return branch->GetEntriesFast() == 0;
    }
}

double MuonSystem::getNumMuons() {
    return muons.size();
}

bool MuonSystem::isMuonTrack(Muon* muon, Track* track) {
    return track->P4().DeltaR(muon->P4()) <= 0.01;
}

bool MuonSystem::isWithinConeP2(Muon* muon, Track* track) {
    return track->P4().DeltaR(muon->P4()) <= 0.2;
}

bool MuonSystem::isIsolated(Muon* muon, std::vector<Track*> tracks) {
    double trackPt = 0;
    for (auto &&track : tracks) {
	if (isWithinConeP2(muon, track) && !isMuonTrack(muon, track)) {
	    trackPt += track->P4().Pt();
	}
    }
    if (trackPt / muon->P4().Pt() < 0.15) {
	return true;
    }
    else {
	return false;
    }
}

void MuonSystem::cutUnisolatedMuons(std::vector<Track*> tracks) {
    for (auto it(muons.begin()); it != muons.end(); ++it) {
	if (!isIsolated(*it, tracks)) {
	    muons.erase(it);
	}
    }
}

bool MuonSystem::isWithinEta(Muon* muon, double eta) {
    return abs(muon->P4().Eta()) < eta;
}

void MuonSystem::cutEta(double eta) {
    for (auto it(muons.begin()); it != muons.end(); ++it) {
	if (!isWithinEta(*it, eta)) {
	    muons.erase(it);
	}
    }
}

bool MuonSystem::areOppositeCharged() {
    if (getNumMuons() == 2) {
	return muons[0]->Charge != muons[1]->Charge;
    }
    return false;
}

double MuonSystem::getDimuonPt() {
    if (getNumMuons() == 2) { 
	return muons[0]->P4().Pt() + muons[1]->P4().Pt();
    }
    return 0;
}

bool MuonSystem::isAbovePtThreshold(double pTThreshold) {
    return getDimuonPt() > pTThreshold;
}

double MuonSystem::getDimuonMass() {
    if (getNumMuons() == 2) {
	return (muons[0]->P4() + muons[1]->P4()).M();
    }
    return 0;
}

bool MuonSystem::isNearZMass() {
    return abs(getDimuonMass() - 91.876) < 8;
}

bool MuonSystem::existsZBoson(std::vector<Track*> tracks, double eta, double pTThreshold) {
    cutUnisolatedMuons(tracks);
    cutEta(eta);
    if (getNumMuons() == 2) {
	if (areOppositeCharged()) {
	    if (isAbovePtThreshold(pTThreshold)) {
		if (isNearZMass()) {
		    return true;
		}
	    }
	}
    }
    return false;
}

std::vector<Muon*> MuonSystem::getMuons() {
    return muons;
}

