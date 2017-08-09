#ifndef ZPRIMEANALYSIS_LIB_JETSYSTEM
#define ZPRIMEANALYSIS_LIB_JETSYSTEM

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#endif

#include <vector>
#include <cstdlib>

#include <TClonesArray.h>
#include <TLorentzVector.h>

class JetSystem {
  public:
    std::vector<Jet*> jets;

    JetSystem();
    JetSystem(TClonesArray* branchJet);
    JetSystem(TClonesArray* branchJet, std::vector<Muon*> muon);
    ~JetSystem();
    
    bool isEmptyBranch(TClonesArray* branch);
    double getNumJets();

    bool isMuonJet(Jet* jet, std::vector<Muon*> muons);
    bool isQuarkJet(Jet* jet, std::vector<GenParticle*> quarks);
    
    TLorentzVector getConstituentsMomentum(Jet* jet);

    std::vector<Jet*> getZPrimeMinJets(std::vector<GenParticle*> quarks);
    std::vector<Jet*> getZPrimeJets(std::vector<GenParticle*> quarks);
    std::vector<Jet*> getNoZPrimeJets(std::vector<Jet*> jetsZPrime);
    
//    void vetoMuons(std::vector<Muon*> muons);
    int vetoMuons(std::vector<Muon*> muons);
    
    bool isWithinEta(Jet* jet, double eta);
    void cutEta(double eta);
    bool isAbovePtThreshold(Jet* jet, double pTThreshold);
    void cutPt(double pTThreshold);
    
    std::vector<Jet*> getJets();

    bool areAbovePtThreshold(double pTThreshold, int numJets);
    double getDeltaEta(std::vector<Jet*> leadingJets);
    bool areBelowDeltaEta(std::vector<Jet*> leadingJets, double eta);
    double getDeltaPhi(std::vector<Jet*> leadingJets);
    bool areAboveDeltaPhi(std::vector<Jet*> leadingJets, double phi);
    double getMass(std::vector<Jet*> leadingJets);
    bool withinMassRange(std::vector<Jet*> leadingJets, double minMass, double maxMass);
    double getDeltaRapidity(std::vector<Jet*> inJets);
    
    TLorentzVector getMomentum();
    TLorentzVector getMomentum(std::vector<Jet*> leadingJets);
    
    std::vector<Jet*> getLeadingJets();
    void removeISR(double dR);

    double getDistanceFromJetAxis(TLorentzVector pJet, TLorentzVector pConstituent);
    double getJetGirth(Jet* jet);

    void sortJets(std::vector<Jet*>* inJets);
};

#endif
