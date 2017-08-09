#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <map>

#include "AtlasUtils.h"
#include "AtlasStyle.h"
#include "AtlasLabels.h"

#ifdef __CLING__
// these are not headers - do not treat them as such - needed for ROOT6
#include "AtlasLabels.C"
#include "AtlasUtils.C"
#endif

#include <TCanvas.h>
#include <TLegend.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TMultiGraph.h>


//int markerstyle[10] = {2, 20, 21, 22, 29, 33, 4, 25, 43, 47};
int markerstyle[3] = {20, 21, 22};
int colorarray[3] = {1, 2, 4};
int lineStyleArray[3] = {1, 8, 10};
void overlayHistograms(std::vector<TH1D*> histograms, TString fileName, TString title, int log=0) {
    TCanvas* canvasOverlay = new TCanvas("canvasOverlay", "canvasOverlay", 1500, 1500);
    TLegend* legendOverlay = new TLegend(0.95, 0.68, 0.60, 0.55);
    legendOverlay->SetTextSize(0.03);
    //    histograms[0]->SetMaximum(histograms[0]->GetMaximum() + histograms[0]->GetMaximum()*0.01);
    histograms[0]->SetMinimum(1e0);
    for (unsigned int i(0); i < histograms.size(); ++i) {
	std::cout << "Hist name: " << histograms[i]->GetName() << "\tEntries: " << histograms[i]->GetEntries() << std::endl;
	histograms[i]->SetTitle(title);
	histograms[i]->SetMarkerStyle(markerstyle[i]);
	histograms[i]->SetMarkerColor(colorarray[i]);
	histograms[i]->SetLineColor(colorarray[i]);
	histograms[i]->SetLineStyle(lineStyleArray[i]);
	histograms[i]->SetMarkerSize(2);
        histograms[i]->SetLineWidth(3);
	histograms[i]->SetStats(0);
	if (i == 0) {
	    histograms[i]->Draw();
	}
	else {
	    histograms[i]->Draw("SAMES");
	}
        legendOverlay -> AddEntry(histograms[i], histograms[i] -> GetName(), "p");
    }
    legendOverlay -> Draw();
    if (log == 1) {
	gPad->SetLogy(1);
    }
    canvasOverlay->SaveAs(fileName);
    std::cout << "Successfully saved!\n" << std::endl;
}

TH1D* getHistogram(TFile* file, TString histName, TString name) {
    
    TH1D* hist = (TH1D*) file->Get(histName);
    std::cout << "Got Histogram: " << hist->GetName() << " from file " << file->GetName() << std::endl;
    hist->SetName(name);   
    return hist;
}

TH1D* getHistogramLumi(TFile* file, TString histName, double luminosity, double xSec, TString name) {
    TH1D* signal =  (TH1D*) file->Get(histName);
    TH1D* histEvents = (TH1D*) file->Get("Events");
    double acceptance = signal->GetEntries() / histEvents->GetEntries();
    signal->Scale(luminosity * xSec / histEvents->GetEntries());
    signal->SetName(name);
    std::cout << name << ": " << luminosity * xSec * acceptance << std::endl;
    return signal;
}

TH1D* getHistogramShape(TFile* file, TString histName, TString name) {
    TH1D* signal = (TH1D*) file->Get(histName);
    double normalTerm = signal->Integral();
    signal->Scale(1/normalTerm);
    signal->SetName(name);
    return signal;
}

void MakePlots(bool isrveto) {
    double luminosity = 36 * 1000;
    TString directory = "~/Work/OutputRoot/";
    TString background = directory + "ZBackground_0to3JetsJet_SIGNALANALYSIS";
    TString signal = directory + "xSecStudy_p5_PYTHIA6Jet_SIGNALANALYSIS";
    if (!isrveto) {
	background += TString("_NOISR");
	signal += TString("_NOISR");
    }
    background += ".root";
    signal += ".root";
    TFile* background1 = new TFile(background, "READ");
    double xSecBackground = 1678;
    TFile* signal5 = new TFile(signal, "READ");
    
    std::vector<double> xSecVector = {0.04794, 0.1889, 0.4271, 0.7524, 1.173, 1.675, 2.287, 2.987, 3.803};

    std::vector<TString> histNames = {"DijetMass", "DijetEta", "DijetPhi", "DijetPt",
				      "LeadingJetPt", "LeadingJetEta",
				      "SubleadingJetPt", "SubleadingJetEta",
				      "Dijet_dPhi_j-j", "Dijet_dEta_j-j", "Dijet_dR_j-j", "Dijet_dY_j-j",
				      "numJetsObjSelect", 
				      "Dijet_Dimuon_dPhi", "Dijet_Dimuon_dEta", "Dijet_Dimuon_dR"};
    std::vector<TString> histTitles = {";m_{jj} [GeV];Entries / 2 GeV", ";#eta_{jj}", ";#phi_{jj}", ";P_{T}^{jj}",
				       ";P_{T}^{j_{leading}} [GeV]", ";#eta_{j_{leading}}",
				       ";P_{T}^{j_{subleading}} [GeV]", ";#eta_{j_{subleading}}",
				       ";#Delta#phi(j,j)", ";#Delta#eta(j,j)", ";#DeltaR(j,j)", ";#Deltay(j,j)",
				       ";# of jets", 
				       ";#Delta#phi(#mu#mu,jj)", ";#Delta#eta(#mu#mu,jj)", "#DeltaR(#mu#mu,jj)"};
    for (unsigned int i = 0; i < histNames.size(); ++i) {
	TString name = histNames[i];
	std::cout << name << std::endl;
	std::vector<TH1D*> hist;
	TString prefix = "~/";
	if (!isrveto) {
	    prefix += TString("NOISR_");
	}
	TString saveName = prefix + name + ".png";
	if (name.Contains("DijetMass")) {
	    hist.push_back(getHistogramLumi(background1, TString("NoCuts_") + name, luminosity, xSecBackground, "Background Calo Jet"));
	    hist.push_back(getHistogramLumi(signal5, TString("MatchedTheJets_")+ name, luminosity, xSecVector[4], "Z' Truth Matched Calo Dijet"));
	    hist.push_back(getHistogramLumi(signal5, "NoCuts_" + name, luminosity, xSecVector[4], "Z' Calo Dijet Candidate"));
	    overlayHistograms(hist, saveName, histTitles[i], 1);
	}
	else if (name.Contains("numJets")) {
	    hist.push_back(getHistogramShape(background1, name, "Background"));
	    hist.push_back(getHistogramShape(signal5, name, "Signal"));
	    overlayHistograms(hist, saveName, histTitles[i]);
	}
	else {
	    hist.push_back(getHistogramShape(background1, TString("NoCuts_") + name, "Background Calo Jet"));
	    hist.push_back(getHistogramShape(signal5, TString("MatchedTheJets_") + name, "Z' Truth Matched Calo Dijet"));
	    hist.push_back(getHistogramShape(signal5, TString("NoCuts_") + name, "Z' Calo Dijet Candidate"));
	    overlayHistograms(hist, saveName, histTitles[i]);
	}
	
    }
}
