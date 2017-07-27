#include "lib/TrackSystem.cc"
#include "lib/JetSystem.cc"
#include "lib/MuonSystem.cc"
#include "lib/TruthSystem.cc"
#include "lib/CutSystem.cc"

#ifdef __CLING__
R__LOAD_LIBRARY(libDelphes)
#include "classes/DelphesClasses.h"
#include "external/ExRootAnalysis/ExRootTreeReader.h"
#endif

#include <iostream>
#include <cmath>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>

#include <TSystem.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TChain.h>
#include <TString.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TDirectory.h>

std::map<TString, TH1D*> hist1DMap;
std::map<TString, TH2D*> hist2DMap;

//  Declare save directory.
std::string saveDirectory = "~/Work/OutputRoot/";

void fillHistogram1D(TString histName, TString histTitle, int nBin, double min, double max, double value, double weight = 1.0) {
    auto it = hist1DMap.find(histName);
    if (it == hist1DMap.end()) {
	hist1DMap[histName] = new TH1D(histName.Data(), histTitle.Data(), nBin, min, max);
    }
    hist1DMap[histName]->Fill(value, weight);
}

void fillHistogram2D(TString histName, TString histTitle, int nBinX, double minX, double maxX, int nBinY, double minY, double maxY, double valueX, double valueY, double weight = 1.0) {
    auto it = hist2DMap.find(histName);
    if (it == hist2DMap.end()) {
        hist2DMap[histName] = new TH2D(histName.Data(), histTitle.Data(), nBinX, minX, maxX, nBinY, minY, maxY);
    }
    hist2DMap[histName]->Fill(valueX, valueY, weight);
}

void saveHistograms(TFile* outFile) {
    std::cout << "Saving histograms" << std::endl;

    TCanvas* c = new TCanvas("c", "c", 1500, 1500);

    for (auto const& entry : hist1DMap) {
        TH1D* histogram = entry.second;
        histogram->Draw();
        histogram->Write();
    }
    for (auto && entry : hist2DMap) {
	TH2D* histogram = entry.second;
	histogram->Draw("colz");
	histogram->Write();
    }
    outFile->Close();
}

void fillEventSelectionVariables(TString eventType, std::vector<double> values) {
    fillHistogram1D(eventType + "_NumMuons", eventType + " Number of Muons", 5, -0.5, 4.5, values[0]);
    fillHistogram1D(eventType + "_MuonsOppositeCharged", eventType + " Muons Opposite Charged Bool", 2, -0.5, 1.5, values[1]);
    fillHistogram1D(eventType + "_MuonsNearZMass", eventType + " Dimuon Mass Residual wrt Z; |M_{Dimuon} - M{Z}| (GeV)", 40, -20, 20, values[2]);    
    fillHistogram1D(eventType + "_numLeadJets", eventType + " Number of Leading Jets", 5, -0.5, 4.5, values[3]);
    fillHistogram1D(eventType + "_deltaEta", eventType + " #Delta #eta between Leading Jets", 50, 0, 6.5, values[4]);
    fillHistogram1D(eventType + "_deltaPhi", eventType + " #Delta #phi between Leading Jets", 50, 0, 6.5, values[5]);
}

void fillCorrelationPlots(TString eventType, std::vector<double> values) {
    fillHistogram2D(eventType + "_Correlation_NumMuons_MuonsOppositeCharged", eventType + " Correlation between Number of Muons and Muons Opposite Charged Bool; number of muons; muons opposite charged", 5, -0.5, 4.5, 2, -0.5, 1.5, values[0], values[1]);
    fillHistogram2D(eventType + "_Correlation_NumMuons_MuonsNearZMass", eventType + " Correlation between Number of Muons and Dimuon Mass Residual wrt Z; number of muons; |M_{Dimuon} - M{Z}| (GeV)", 5, -0.5, 4.5, 40, 0, 20, values[0], values[2]);
    fillHistogram2D(eventType + "_Correlation_NumMuons_numLeadJets", eventType + " Correlation between Number of Muons and Number of Leading Jets; number of muons; number of leading jets", 5, -0.5, 4.5, 5, -0.5, 4.5, values[0], values[3]);
    fillHistogram2D(eventType + "_Correlation_NumMuons_deltaEta", eventType + " Correlation between Number of Muons and #Delta#eta between Leading Jets; number of muons; |#Delta#eta_{jj}|", 5, -0.5, 4.5, 50, 0, 6.5, values[0], values[4]);
    fillHistogram2D(eventType + "_Correlation_NumMuons_deltaPhi", eventType + " Correlation between Number of Muons and #Delta#phi between Leading Jets; number of muons; |#Delta#phi_{jj}|", 5, -0.5, 4.5, 50, 0, 6.5, values[0], values[5]);

    fillHistogram2D(eventType + "_Correlation_MuonsNearZMass_numLeadJets", eventType + " Correlation between Dimuon Mass Residual wrt Z and number of Leading Jets; |M_{Dimuon} - M{Z}| (GeV); number of leading jets", 40, 0, 20, 5, -0.5, 4.5, values[2], values[3]);
    fillHistogram2D(eventType + "_Correlation_MuonsNearZMass_deltaEta", eventType + " Correlation between Dimuon Mass Residual wrt Z and #Delta#eta between Leading Jets; |M_{Dimuon} - M{Z}| (GeV); |#Delta#eta_{jj}|", 40, 0, 20, 50, 0, 6.5, values[2], values[4]);
    fillHistogram2D(eventType + "_Correlation_MuonsNearZMass_deltaPhi", eventType + " Correlation between Dimuon Mass Residual wrt Z and #Delta#phi between Leading Jets; |M_{Dimuon} - M{Z}| (GeV); |#Delta#phi_{jj}|", 40, 0, 20, 50, 0, 6.5, values[2], values[5]);

    fillHistogram2D(eventType + "_Correlation_deltaEta_deltaPhi", eventType + " Correlation between #Delta#eta and #Delta#phi between Leading Jets; |#Delta#eta_{jj}|; |#Delta#phi_{jj}|", 50, 0, 6.5, 50, 0, 6.5, values[4], values[5]);
}

void fillKinematics(TString eventType, TLorentzVector pMuons, TLorentzVector pLeadingJets, std::vector<Muon*> muons) {
    fillHistogram1D(eventType + "_MuonsEta", "Muons #eta; #eta", 40, -5, 5, pMuons.Eta());
    fillHistogram1D(eventType + "_MuonsPt", "Muons P_{T}; P_{T} (GeV)", 100, 0, 300, pMuons.Pt());
    fillHistogram1D(eventType + "_MuonsMass", "Muons Mass; mass (GeV)", 100, 0, 200, pMuons.M());
    fillHistogram1D(eventType + "_MuonsPhi", "Muons #phi; #phi", 40, -3.5, 3.5, pMuons.Phi());

    fillHistogram1D(eventType + "_LJetsEta", "Leading Jets #eta; #eta", 40, -8, 8, pLeadingJets.Eta());
    fillHistogram1D(eventType + "_LJetsPt", "Leading Jets P_{T}; P_{T} (GeV)", 100, 0, 300, pLeadingJets.Pt());
    fillHistogram1D(eventType + "_LJetsMass", "Leading Jets mass; mass (GeV)", 100, 0, 200, pLeadingJets.M());
    fillHistogram1D(eventType + "_LJetsPhi", "Leading Jets phi, #phi", 40, -3.5, 3.5, pLeadingJets.Phi());

    for (unsigned int i = 0; i < muons.size(); ++i) {
	TString type = "Leading";
	if (i == 2) {
	    type = "Subleading";
	}
	fillHistogram1D(eventType + "_" + type + "MuonEta", type + "Muon #eta; #eta", 40, -5, 5, muons[i]->P4().Eta());
	fillHistogram1D(eventType + "_" + type + "MuonPt", type + "Muon P_{T}; P_{T} (GeV)", 100, 0, 300, muons[i]->P4().Eta());
	fillHistogram1D(eventType + "_" + type + "MuonMass", type + "Muon mass; mass (GeV)", 100, 0, 200, muons[i]->P4().Eta());
	fillHistogram1D(eventType + "_" + type + "MuonPhi", type + "Muon #phi; #eta", 40, -3.5, 3.5, muons[i]->P4().Eta());
    }
}

void fillCutFlow(CutSystem* cutSystem, int numCuts) {
    for (int i = 0; i < numCuts; ++i) {
	if (cutSystem->isCutFailed(i)) {
	    return;
	}
	else {
	    fillHistogram1D("CutFlow", "CutFlow", 7, -0.5, 6.5, i);;
	}
    }
}	

void fillNMinus1Plots(CutSystem* cutSystem, std::vector<double> values, std::vector<TString> eventTypes) {
    for (unsigned int i = 0; i < eventTypes.size(); ++i) {
	if (cutSystem->maskCut(i) == 0) {
	    fillEventSelectionVariables(eventTypes[i] + "_NMinus1", values);
	}
    }
}

void fillNMinus2Plots(CutSystem* cutSystem, std::vector<double> values, std::vector<TString> eventTypes) {
    std::cout << "Size of values: " << values.size() << std::endl;
    for (unsigned int i = 0; i < values.size(); ++i) {
	for (unsigned int j = i + 1; j < values.size(); ++j) {
	    if (cutSystem->maskCuts({i, j}) == 0) {
		fillCorrelationPlots(eventTypes[i] + "_" + eventTypes[j] + "_NMinus2", values);
	    }
	}
    }
}

void SignalAnalysis(TString inputFileDirectory, bool signal, std::vector<double> eventCriteria) {
    TChain chain("Delphes");
    TString inputFileName = "~/Work/MadGraph/" + inputFileDirectory + "/Events/run_01/tag_1_delphes_events.root";
    chain.Add(inputFileName);
    
    //  Create object of class ExRootTreeReader.
    ExRootTreeReader* treeReader = new ExRootTreeReader(&chain);
    Long64_t numberOfEntries = treeReader -> GetEntries();
    
    //  Get pointers to branches used in this analysis.
    TClonesArray* branchMuon = treeReader->UseBranch("Muon");
    TClonesArray* branchJet = treeReader->UseBranch("Jet");
    TClonesArray* branchTrack = treeReader->UseBranch("Track");
    TClonesArray* branchParticle = treeReader->UseBranch("Particle");

    std::vector<TString> eventSelection = {"Muon_DimuonCut", "Muon_OppositeChargeCut", "Muon_NearZMassCut", "Jet_TwoLeadingCut", "Jet_DeltaEtaCut", "Jet_DeltaPhiCut"};
    
    TString outfileNameFull = saveDirectory + inputFileDirectory + "_SIGNALANALYSIS.root";
    TFile* outFile = new TFile(outfileNameFull, "RECREATE");

    for(Int_t entry = 4000; entry < numberOfEntries; ++entry) {
	treeReader -> ReadEntry(entry);
	if (entry % 100 == 0) {
	    std::cout << "Event " << entry << " processed!"  << std::endl;
	}
	fillHistogram1D("Events", "numEvents", 3, -0.5, 2.5, 1);

	std::vector<GenParticle*> quarksZPrime;
	if (signal) {
	    TruthSystem* truthSystem = new TruthSystem(branchParticle);
	    quarksZPrime = truthSystem->getZPrimeQuarks();
	    fillHistogram1D("DeltaRZPrimeQuarks", "Delta R between Z' Quarks; #DeltaR", 100, 0, 7.85, quarksZPrime[0]->P4().DeltaR(quarksZPrime[1]->P4()));
	}
	
	TrackSystem* trackSystem = new TrackSystem(branchTrack);
	std::vector<Track*> tracks = trackSystem->getTracks();
	fillHistogram1D("numTracks", "Number of Tracks per Event", 51, -0.5, 50.5, trackSystem->getNumTracks());
	
	MuonSystem* muonSystem = new MuonSystem(branchMuon);
	fillHistogram1D("numMuons", "Number of Muons per Event", 6, -0.5, 5.5, muonSystem->getNumMuons());
	//  Object selection.
	muonSystem->cutUnisolatedMuons(tracks);
	muonSystem->cutPt(24);
	muonSystem->cutEta(2.4);
	fillHistogram1D("numMuonsObjSelect", "Number of Muons per Event after Object Selection", 6, -0.5, 5.5, muonSystem->getNumMuons());
	std::vector<Muon*> muons = muonSystem->getMuons();
	TLorentzVector pMuons = muonSystem->getMomentum();
	
	JetSystem* jetSystem = new JetSystem(branchJet);
	fillHistogram1D("numJets", "Number of Jets per Event", 10, -0.5, 9.5, jetSystem->getNumJets());
	// Object selection.
	int numMuonJets = jetSystem->vetoMuons(muons);
        fillHistogram1D("numMuonJets", "Number of Muon Jets per Event", 5, -0.5, 4.5, numMuonJets);
	jetSystem->cutEta(2.8);
	jetSystem->cutPt(25);
	fillHistogram1D("numJetsObjSelect", "Number of Jets per Event after Object Selection", 10, -0.5, 9.5, jetSystem->getNumJets());
	std::vector<Jet*> matchedJets = jetSystem->getZPrimeJets(quarksZPrime);
	if (matchedJets.size() == 2) {
            fillHistogram1D("matchedJets", "Z' ID'd Dijet Mass; m_{jj}; N", 200, 0, 400, jetSystem->getMass(matchedJets));
        }
	
	std::vector<Jet*> leadingJets = jetSystem->getLeadingJets();
	std::vector<double> values = {static_cast<double>(muonSystem->getNumMuons()), static_cast<double>(muonSystem->areOppositeCharged()),
				      muonSystem->distanceToZMass(), static_cast<double>(leadingJets.size()), jetSystem->getDeltaEta(leadingJets), jetSystem->getDeltaPhi(leadingJets)};
	
	//  Event selection.
	CutSystem* cutSystem = new CutSystem();
	fillEventSelectionVariables("NoCuts", values);
	fillCorrelationPlots("NotCuts", values);
	
	if (muonSystem->getNumMuons() == eventCriteria[0]) {
	    fillEventSelectionVariables(eventSelection[0], values);
	}
	else {
	    cutSystem->setCutFailed(0);
	}

	if (muonSystem->areOppositeCharged() == eventCriteria[1]) {
	    fillEventSelectionVariables(eventSelection[1], values);
	}
	else {
	    cutSystem->setCutFailed(1);
	}

	if (muonSystem->isNearZMass(eventCriteria[2])) {
	    fillEventSelectionVariables(eventSelection[2], values);   
	}
	else {
	    cutSystem->setCutFailed(2);
	}
	if (leadingJets.size() == eventCriteria[3]) {
	    fillEventSelectionVariables(eventSelection[3], values);
	}
	else {
            cutSystem->setCutFailed(3);
	}
	if (jetSystem->areBelowDeltaEta(leadingJets, eventCriteria[4])) {
	    fillEventSelectionVariables(eventSelection[4], values);
	}
	else {
	    cutSystem->setCutFailed(4);
	}

	if (jetSystem->areAboveDeltaPhi(leadingJets, eventCriteria[5])) {
	    fillEventSelectionVariables(eventSelection[5], values);
	}
	else {
	    cutSystem->setCutFailed(5);
	}
	
	if (cutSystem->getCuts() == 0) {	    
	    fillHistogram1D("LeadingJets2", "Leading Dijet Mass After Secondary Cuts; m_{jj}; N", 40, 0, 400, jetSystem->getMass(leadingJets));
	    fillEventSelectionVariables("All", values);
	}

	fillNMinus1Plots(cutSystem, values, eventSelection);
	fillNMinus2Plots(cutSystem, values, eventSelection);
	fillCutFlow(cutSystem, eventSelection.size());
    }
    saveHistograms(outFile);
}
