#ifndef ZPRIMEANALYSIS_LIB_TRACKSYSTEM
#define ZPRIMEANALYSIS_LIB_TRACKSYSTEM

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#endif

#include <vector>

#include <TSystem.h>
#include <TClonesArray.h>

class TrackSystem {
  public:
    std::vector<Track*> tracks;

    TrackSystem();
    TrackSystem(TClonesArray* branchTrack);
    ~TrackSystem();

    bool isEmptyBranch(TClonesArray* branch);
    double getNumTracks();
    std::vector<Track*> getTracks();
    TLorentzVector getMomentum();
    TLorentzVector getMomentum(std::vector<Muon*> muons);
    
    bool isMuonTrack(Track* track, std::vector<Muon*> muons);
    void vetoMuons(std::vector<Muon*> muons);

    bool isJetTrack(Track* track, std::vector<Jet*> jets);
    void vetoJets(std::vector<Jet*> jets);
};

#endif
