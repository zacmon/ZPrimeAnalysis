#include "MuonSystem.h"

#include <cmath>

#include <TLorentzVector.h>

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
    unsigned int i = 0;
    while (i < muons.size()) {
	if (!isIsolated(muons[i], tracks)) {
	    muons.erase(muons.begin() + i);
	}
	else {
            i++;
	}
    }
}

bool MuonSystem::isWithinEta(Muon* muon, double eta) {
    return std::abs(muon->P4().Eta()) < eta;
}

void MuonSystem::cutEta(double eta) {
    unsigned int i = 0;
    while (i < muons.size()) {
        if (!isWithinEta(muons[i], eta)) {
            muons.erase(muons.begin() + i);
	}
    	else {
            i++;
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
	return (muons[0]->P4() + muons[1]->P4()).Pt();
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
    return std::abs(getDimuonMass() - 91.876) < 8;
}

bool MuonSystem::existsZBoson(std::vector<Track*> tracks, double eta, double pTThreshold) {
    cutUnisolatedMuons(tracks);
    cutEta(eta);
    if (getNumMuons() == 2) {
	if (areOppositeCharged() && isAbovePtThreshold(pTThreshold) && isNearZMass()) {
	    return true;
	}
    }
    return false;
}

std::vector<Muon*> MuonSystem::getMuons() {
    return muons;
}
