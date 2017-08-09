#include "../interface/TrackSystem.h"

#include <cstdlib>

#include <TLorentzVector.h>

TrackSystem::TrackSystem() {
}

TrackSystem::TrackSystem(TClonesArray* branchTrack) {
    if (!isEmptyBranch(branchTrack)) {
        for (int i(0); i < branchTrack->GetEntriesFast(); ++i) {
            Track* track  = (Track*) branchTrack->At(i);
            tracks.push_back(track);
        }
    }
}

TrackSystem::~TrackSystem() {
}

bool TrackSystem::isEmptyBranch(TClonesArray* branch) {
    if (!branch) {
        return 0;
    }
    else {
        return branch->GetEntries() == 0;
    }
}

double TrackSystem::getNumTracks() {
    return tracks.size();
}

std::vector<Track*> TrackSystem::getTracks() {
    return tracks;
}

TLorentzVector TrackSystem::getMomentum() {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&track : tracks) {
	momentum += track->P4();
    }
    return momentum;
}

TLorentzVector TrackSystem::getMomentum(std::vector<Muon*> muons) {
    TLorentzVector momentum(0.0, 0.0, 0.0, 0.0);
    for (auto &&track : tracks) {
	momentum += track->P4();
    }
    for (auto &&muon : muons) {
	momentum += muon->P4();
    }
    return momentum;
}

bool TrackSystem::isMuonTrack(Track* track, std::vector<Muon*> muons) {
    if (muons.size() == 0) {
	return false;
    }
    for (auto &&muon : muons) {
	if (track->P4().DeltaR(muon->P4()) < 0.05) {
	    return true;
	}
    }
    return false;
}

void TrackSystem::vetoMuons(std::vector<Muon*> muons) {
    unsigned int i = 0;
    while (i < tracks.size()) {
        if (isMuonTrack(tracks[i], muons)) {
            tracks.erase(tracks.begin() + i);
        }
        else {
            ++i;
        }
    }
}

bool TrackSystem::isJetTrack(Track* track, std::vector<Jet*> jets) {
    if (jets.size() == 0) {
	return false;
    }
    for (auto &&jet : jets) {
        if (track->P4().DeltaR(jet->P4()) < 0.5) {
            return true;
	}
    }
    return false;
}


void TrackSystem::vetoJets(std::vector<Jet*> jets) {
    unsigned int i = 0;
    while (i < tracks.size()) {
	if (isJetTrack(tracks[i], jets)) {
	    tracks.erase(tracks.begin() + i);
	}
	else {
	    ++i;
	}
    }
}
