#include "../interface/MuonSystem.h"

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

int MuonSystem::getNumMuons() {
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

bool MuonSystem::isAbovePt(Muon* muon, double pT) {
    return std::abs(muon->P4().Pt()) > pT;
}

void MuonSystem::cutPt(double pT) {
    unsigned int i = 0;
    while (i < muons.size()) {
        if (!isAbovePt(muons[i], pT)) {
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

TLorentzVector MuonSystem::getMomentum() {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&muon : muons) {
	momentum += muon->P4();
    }
    return momentum;
}

double MuonSystem::getDimuonPt() {
    if (getNumMuons() == 2) {
	return getMomentum().Pt();
    }
    return -9999;
}

double MuonSystem::getDimuonMass() {
    if (getNumMuons() == 2) {
	return getMomentum().M();
    }
    return -999;
}

bool MuonSystem::isAbovePtThreshold(double pTThreshold) {
    return getDimuonPt() > pTThreshold;
}

double MuonSystem::distanceToZMass() {
    return std::abs(getMomentum().M() - 91.876);
}

bool MuonSystem::isNearZMass(double distance) {
    return distanceToZMass() < distance;
}

bool MuonSystem::existsZBoson(std::vector<Track*> tracks, double eta, double pTThreshold) {
    cutUnisolatedMuons(tracks);
    cutEta(eta);
    if (getNumMuons() == 2) {
	if (areOppositeCharged() && isAbovePtThreshold(pTThreshold) && isNearZMass(8)) {
	    return true;
	}
    }
    return false;
}

std::vector<Muon*> MuonSystem::getMuons() {
    return muons;
}

void MuonSystem::sortMuonsByPt(std::vector<Muon*>* inMuons) {
    std::sort(inMuons->begin(), inMuons->end(), [](Muon* muon1, Muon* muon2) {return muon1->P4().Pt() > muon2->P4().Pt();});
}
