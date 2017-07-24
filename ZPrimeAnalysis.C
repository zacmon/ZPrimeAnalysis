#include <lib/JetSystem.h>
#include <lib/MuonSystem.h>


#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

#include <iostream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <algorithm>

#include <TSystem.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TChain.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TCanvas.h>
#include <TObject.h>
#include <TLegend.h>



void ZPrimeAnalysis () {
    TString inputFile =  "/Users/zacmon/Work/MadGraph/Z_Background/Events/run_01/tag_1_delphes_events.root";
    TChain chain("Delphes");
    chain.Add(inputFile);

    //  Create object of class ExRootTreeReader.
    ExRootTreeReader* treeReader = new ExRootTreeReader(&chain);
    Long64_t numberOfEntries = treeReader -> GetEntries();

    //  Get pointers to branches used in this analysis.
    TClonesArray* branchMuon = treeReader->UseBranch("Muon");
    TClonesArray* branchJet = treeReader->UseBranch("Jet");
    //    TClonesArray* branchTrack = treeReader->UseBranch("Track");

    std::cout << numberOfEntries << std::endl;
    for(Int_t entry = 0; entry < numberOfEntries; ++entry) {
        treeReader -> ReadEntry(entry);
	
        if (entry % 100 == 0) {
            std::cout << "." << std::flush;
        }

	MuonSystem* muonSystem = new MuonSystem(branchMuon);
	JetSystem* jetSystem = new JetSystem(branchJet);
	
        std::vector<Jet*> jets = getJets(branchJet, branchMuon);
        std::vector<Jet*> leadingJets;
        for (int i(0); i < jets.size(); ++i) {
             leadingJets.push_back(jets[i]);
            if (leadingJets.size() == 2) {
                fillJetSignalHistogram(leadingJets);
                break;
            }
	}
    }
}
