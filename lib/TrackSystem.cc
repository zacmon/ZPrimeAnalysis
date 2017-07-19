#include "TrackSystem.h"

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

