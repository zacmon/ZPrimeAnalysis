#ifndef ZPRIMEANALYSIS_LIB_MUONSYSTEM
#define ZPRIMEANALYSIS_LIB_MUONSYSTEM

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

#include <vector>

#include <TSystem.h>
#include <TClonesArray.h>

class MuonSystem {
  public:
    MuonSystem(TClonesArray* branchMuon);
    ~MuonSystem();
    
    bool isEmptyBranch(TClonesArray* branch);
    
    double getNumMuons();
    
    bool isMuonTrack(Muon* muon, Track* track);
    bool isWithinConeP2(Muon* muon, Track* track);
    bool isIsolated(Muon* muon, std::vector<Track*> tracks);
    void cutUnisolatedMuons(std::vector<Track*> tracks);
    
    bool isWithinEta(Muon* muon, double eta);
    void cutEta(double eta);
    
    bool areOppositeCharged();
    
    double getDimuonPt();
    bool isAbovePtThreshold(double pTThreshold);

    double getDimuonMass();
    bool isNearZMass();
    bool existsZBoson(std::vector<Track*> tracks, double eta, double pTThreshold);
    
    bool isZPrime();
    
    std::vector<Muon*> getMuons();

  private:
    std::vector<Muon*> muons;
};

#endif
