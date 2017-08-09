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
int markerstyle[3] = {24, 25, 26};
int colorarray[3] = {1, 2, 4};
int lineStyleArray[3] = {1, 8, 10};
void overlayHistograms(std::vector<TH1D*> histograms, TString fileName, int log=0) {
    TCanvas* canvasOverlay = new TCanvas("canvasOverlay", "canvasOverlay", 1500, 1500);
    TLegend* legendOverlay = new TLegend(0.85, 0.85, 0.70, 0.75);
    //    histograms[0]->SetMaximum(0.08);
    histograms[0]->SetMinimum(1e0);
    for (unsigned int i(0); i < histograms.size(); ++i) {
	std::cout << "Hist name: " << histograms[i]->GetName() << "\tEntries: " << histograms[i]->GetEntries() << std::endl;
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
    canvasOverlay -> SaveAs(fileName);
}

TH1D* getHistogram(TFile* file, TString histName, TString name) {
    std::cout << "File: " << file->GetName() << "\tHistName: " << histName << std::endl;
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

std::vector<int> getOneSigmaBins(TH1D* histogram) {
    int modeBin = histogram->GetMaximumBin();
    int totalBins = histogram->GetNbinsX();
    double totalIntegral = histogram->Integral();
    double partialIntegral = histogram->Integral(modeBin, modeBin);
    double tempPartialIntegral = 0;
    int leftCounter = modeBin;
    int rightCounter = modeBin;
    double leftBinContent = 0;
    double rightBinContent = 0;
    while (partialIntegral / totalIntegral < 0.6827) {
	int nextLeft = leftCounter - 1;
	int nextRight = rightCounter + 1;
	if (nextLeft > 0) {
	    leftBinContent = histogram->GetBinContent(nextLeft);
	}
	if (nextRight < totalBins) {
	    rightBinContent = histogram->GetBinContent(nextRight);
	}
	if (leftBinContent > rightBinContent) {
	    leftCounter = nextLeft;
	}
	else {
	    rightCounter = nextRight;
	}
	tempPartialIntegral = histogram->Integral(leftCounter, rightCounter);
	if (std::abs(partialIntegral/totalIntegral - 0.6827) > std::abs(tempPartialIntegral/totalIntegral - 0.6827)) {
	    partialIntegral = tempPartialIntegral;
	}
	else {
	    break;
	}
    }
    return {leftCounter, rightCounter};
}

double getBackground(TH1D* histogram, double minX, double maxX) {
    int minBin = histogram->FindBin(minX);
    int maxBin = histogram->FindBin(maxX);
    return histogram->Integral(minBin, maxBin);
}

void ReadRootFile() {
    double luminosity = 20.3 * 1000;
    TString directory = "~/Work/OutputRoot/";
    TFile* background1 = new TFile(directory + "ZBackground_0to3JetsJet_SIGNALANALYSIS.root", "READ");
    //TFile* background2 = new TFile(directory + "ZBackground_1to3JetsJet_SIGNALANALYSIS.root", "READ");
    double xSecBackground = 1678;
    TFile* signal5 = new TFile(directory + "xSecStudy_p5_PYTHIA6Jet_SIGNALANALYSIS.root", "READ");
    
    std::vector<TH1D*> histograms;
    std::vector<TH1D*> histNorm;
    //    histNorm.push_back(getHistogramShape(background1, "ZBackground_0to3"));
    histograms.push_back(getHistogramLumi(background1, "NoCuts_DijetMass", luminosity, xSecBackground, "ZBackground_0to3"));
    //    histNorm.push_back(getHistogramsNormalized(background2, "ZBackground_1to3"));
    std::cout << "----- Obtained Z Background Data -----" << std::endl;
    
    std::vector<double> xSecVector = {0.04794, 0.1889, 0.4271, 0.7524, 1.173, 1.675, 2.287, 2.987, 3.803};
    //TFile* jetSignal = new TFile(directory + "xSecStudy_p5Jet_SIGNALANALYSIS.root", "READ");
    //histNorm.push_back(getHistogramsNormalized(jetSignal, "Jet Signal"));
    // for (unsigned int i = 0; i < xSecVector.size(); ++i) {
    // 	TString number = Form("%d", i+1);
    //  	TString fileName = directory + "xSecStudy_p" + number + "Jet_SIGNALANALYSIS.root";
    // 	if (i == 4) {
    // 	    fileName = directory + "xSecStudy_p" + number + "_PYTHIA6Jet_SIGNALANALYSIS.root";
    // 	}
    // 	std::cout << fileName << std::endl;
    // 	TFile* file = new TFile(fileName, "READ");
    // 	histograms.push_back(getHistogramLumi(file, "NoCuts_DijetMass", luminosity, xSecVector[i], "xSecStudy_p" + number));
    // 	if (i == 4) {
    // 	    histNorm.push_back(getHistogramShape(file, "Signal_50GeV_xSec0p" + number));
    // 	}
    // 	std::cout << "----- Obtained xSecStudy_p" << i + 1 << " Data -----" << std::endl;
    //  }

    // TGraph* gSignal = new TGraph();
    // TGraph* gBackground = new TGraph();

    // for (unsigned int i = 1; i < 10; i++) {//histograms.size(); ++i) {
    // 	std::vector<int> bins = getOneSigmaBins(histograms[i]);
    // 	std::cout << i << std::endl;
    // 	std::cout << "Getting one sigma bins" << std::endl;
    // 	double signal = histograms[i]->Integral(bins[0], bins[1]);
    // 	std::cout << "Getting signal yield" << std::endl;
    // 	double minX = histograms[i]->GetBinCenter(bins[0]);
    // 	double maxX = histograms[i]->GetBinCenter(bins[1]);
    // 	double yieldLimit = 1.9 * std::sqrt(getBackground(histograms[0], minX, maxX));
    // 	std::cout << "Getting yield limit" << std::endl;
    // 	gSignal->SetPoint(i, 0.1 * i, signal);
    // 	gBackground->SetPoint(i, 0.1 * i, yieldLimit);
    // 	std::cout << "Set points" << std::endl;
    // }
    // std::cout << "Setting attributes 1" << std::endl;
    // gSignal->SetLineStyle(1);
    // gSignal->SetMarkerStyle(20);
    // gSignal->SetMarkerSize(3);
    // gSignal->SetLineWidth(5);
    // std::cout << "Going to remove point" << std::endl;
    // gSignal->RemovePoint(0);
    // std::cout << "Removed point" << std::endl;
    // gBackground->SetLineStyle(2);
    // gBackground->SetMarkerStyle(21);
    // gBackground->SetLineWidth(5);
    // gBackground->SetMarkerSize(3);
    // gBackground->RemovePoint(0);
    // std::cout << "Set attributes" << std::endl;
    // TMultiGraph* mg = new TMultiGraph();
    // TLegend* legend = new TLegend(0.5, 0.52, 0.7, 0.65);
    // legend->AddEntry(gSignal, "Signal Yield", "l");
    // legend->AddEntry(gBackground, "Signal Yield Limit", "l");
    // mg->Add(gSignal);
    // mg->Add(gBackground);
    // TCanvas* c = new TCanvas("c", "c", 1500, 1500);
    // mg->Draw("APL");
    // mg->SetTitle("M_{Z'} = 50: Yield Limit vs. g_{SM}");
    // mg->GetXaxis()->SetTitle("g_{SM}");
    // mg->GetYaxis()->SetTitle("Signal Yield");
    // mg->GetYaxis()->SetTitleOffset(1.4);
    // legend->Draw();
    // gPad->Modified();

    std::vector<TString> histNames = {"DijetMass", "deltaEta"};
    for (auto &&name : histNames) {
	std::vector<TH1D*> hist(3);
	if (name.Contains("DijetMass")) {
	    hist[0] = getHistogramLumi(background1, TString("NoCuts_") + name, luminosity, xSecBackground, "Background Calo Jet");
	    hist[1] = getHistogramLumi(signal5, TString("MatchedTheJets_")+ name, luminosity, xSecVector[4], "Z' Truth Matched Calo Dijet");
	    hist[2] = getHistogramLumi(signal5, "NoCuts_" + name, luminosity, xSecVector[4], "Z' Calo Dijet Candidate");
	    overlayHistograms(hist, "~/" + name + ".png", 1);
	}
	else {
	    hist[0] = getHistogramShape(background1, TString("NotCuts_") + name, "Background Calo Jet");
	    hist[1] = getHistogramShape(signal5, TString("MatchedTheJets_") + name, "Z' Truth Matched Calo Dijet");
	    hist[2] = getHistogramShape(signal5, TString("NoCuts_") + name, "Z' Calo Dijet Candidate");
	    overlayHistograms(hist, "~/" + name + ".png");
	}
    }
    
    
    //overlayHistograms(histNorm, "~/signalNorm.png");
    //    overlayHistograms({histograms[0], histograms[5]}, "~/signalOverlay.png");
}
