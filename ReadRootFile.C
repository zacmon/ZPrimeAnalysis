#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <map>

#include <TCanvas.h>
#include <TLegend.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TGraph.h>
#include <TMultiGraph.h>

int markerstyle[10] = {2, 20, 21, 22, 29, 33, 4, 25, 43, 47};

void overlayHistograms(std::vector<TH1D*> histograms, TString fileName) {
    TCanvas* canvasOverlay = new TCanvas("canvasOverlay", "canvasOverlay", 1500, 1500);
    TLegend* legendOverlay = new TLegend(0.32, 0.27, 0.2, 0.15);
    //    histograms[0]->SetMinimum(1e-1);
    for (unsigned int i(0); i < histograms.size(); ++i) {
	histograms[i]->Draw("SAMES");
	histograms[i]->SetLineColor(0);//(i%9 + 1);
	histograms[i]->SetLineStyle(i/9 + 1);
	histograms[i]->SetMarkerStyle(20);
	histograms[i]->SetMarkerSize(2);
	if (i < 10) {
	    histograms[i]->SetMarkerColor(i%9 + 1);
	}
	if (i > 10) {
	    histograms[i]->SetMarkerColor(38);
	}
        histograms[i]->SetLineWidth(3);
        legendOverlay -> AddEntry(histograms[i], histograms[i] -> GetName(), "p");
    }
    legendOverlay -> Draw();
    //gPad->SetLogy(1);
    canvasOverlay -> SaveAs(fileName);
}

TH1D* getHistogram(TFile* file, double luminosity, double xSec, TString name) {
    TH1D* signal =  (TH1D*) file->Get("LeadingJets2");
    TH1D* histEvents = (TH1D*) file->Get("Events");
    double acceptance = signal->GetEntries() / histEvents->GetEntries();
    signal->Scale(luminosity * xSec / histEvents->GetEntries());
    signal->SetName(name);
    std::cout << name << ": " << luminosity * xSec * acceptance << std::endl;
    return signal;
}

TH1D* getHistogramsNormalized(TFile* file, TString name) {
    TH1D* signal = (TH1D*) file->Get("LeadingJets2");
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
    TFile* background = new TFile(directory + "Z_Background.root", "READ");
    double xSecBackground = 1678;
    
    std::vector<TH1D*> histograms;
    histograms.push_back(getHistogram(background, luminosity, xSecBackground, "Z_Background"));
    std::vector<TH1D*> histNorm;
    std::vector<double> xSecVector = {0.04794, 0.1889, 0.4271, 0.7524, 1.173, 1.675, 2.287, 2.987, 3.803};
    for (unsigned int i = 0; i < xSecVector.size(); ++i) {
	TString number;
	number.Form("%d", i+1);
	TString fileName = directory + "xSecStudy_p" + number + ".root";
	TFile* file = new TFile(fileName, "READ");
	histograms.push_back(getHistogram(file, luminosity, xSecVector[i], "xSecStudy_p" + number));
	histNorm.push_back(getHistogramsNormalized(file, "xSecStudy_p" + number));
    }

    TGraph* gSignal = new TGraph();
    TGraph* gBackground = new TGraph();
    
    for (unsigned int i = 1; i < 10; ++i) {//histograms.size(); ++i) {
	std::vector<int> bins = getOneSigmaBins(histograms[i]);
	double signal = histograms[i]->Integral(bins[0], bins[1]);
	double minX = histograms[i]->GetBinCenter(bins[0]);
	double maxX = histograms[i]->GetBinCenter(bins[1]);
	double yieldLimit = 1.9 * std::sqrt(getBackground(histograms[0], minX, maxX));

	gSignal->SetPoint(i, 0.1 * i, signal);
	gBackground->SetPoint(i, 0.1 * i, yieldLimit);
    }
    
    gSignal->SetLineStyle(1);
    gSignal->SetMarkerStyle(20);
    gSignal->SetMarkerSize(3);
    gSignal->SetLineWidth(5);
    gSignal->RemovePoint(0);
    gBackground->SetLineStyle(2);
    gBackground->SetMarkerStyle(21);
    gBackground->SetLineWidth(5);
    gBackground->SetMarkerSize(3);
    gBackground->RemovePoint(0);
    TMultiGraph* mg = new TMultiGraph();
    TLegend* legend = new TLegend(0.5, 0.52, 0.7, 0.65);
    legend->AddEntry(gSignal, "Signal Yield", "l");
    legend->AddEntry(gBackground, "Signal Yield Limit", "l");
    mg->Add(gSignal);
    mg->Add(gBackground);
    TCanvas* c = new TCanvas("c", "c", 1500, 1500);
    mg->Draw("APL");
    mg->SetTitle("M_{Z'} = 50: Yield Limit vs. g_{SM}");
    mg->GetXaxis()->SetTitle("g_{SM}");
    mg->GetYaxis()->SetTitle("Signal Yield");
    mg->GetYaxis()->SetTitleOffset(1.4);
    legend->Draw();
    gPad->Modified();

    overlayHistograms(histNorm, "~/signalNorm.png");
    //    overlayHistograms(histograms, "~/signalOverlay.png");
}
