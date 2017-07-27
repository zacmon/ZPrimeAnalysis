#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include <TFile.h>
#include <TH1D.h>

TH1D* getHistogram(TFile* file, double luminosity, double xSec, TString name) {
    TH1D* signal = (TH1D*)file->Get("");
    TH1D* events = (TH1D*)file->Get("Events");
    double acceptance = signal->GetEntries() / histEvents->GetEntries();
    signal->Scale(luminosity * xSec / histEvents->GetEntries());
    signal->SetName();
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

double getBackgroundYield(TH1D* histogram, std::vector<int> bounds) {
    int minBin = histogram->FindBin(bounds[0]);
    int maxBin = histogram->FindBin(bounds[1]);
    return histogram->Integral(minBin, maxBin);
}

std::vector<double> getBounds(TH1D* histogram, std::vector<int> bins) {
    double minX = histogram->GetBinCenter(bins[0]);
    double maxX = histogram->GetBinCenter(bins[1]);
    return {minX, maxX};
}

double Significance(TString signalFileName, TString backgroundFileName) {
    TString directory = "~/Work/OutputRoot/";
    TFile* signalFile = new TFile(directory + signalFileName + ".root", "READ");
    TFile* backgroundFile = new TFile(directory + backgroundFileName + ".root", "READ");

    double xSecBackground = 1678;
    TH1D* backgroundHist = getHistogram(backgroundFile, luminosity, xSecBackground, "Z_Background");

    double xSecSignal = 1.177;
    TH1D* signalHist = getHistogram(signalFile, luminosity, xSecSignal, "Z_Signal_p5");

    std::vector<int> bins = getOneSigmaBins(signalHist);
    double signal = signalHist->Integral(bins[0], bins[1]);
    std::vector<double> bounds = getBounds(signalHist, bins);
    double backgroundYield = getBackgroundYield(backgroundHist, bounds)
    double backgroundUncertainty = std::sqrt(backgroundYield);
    double yieldLimit = 1.9 * backgroundUncertainty;

    return signal / backgroundYield;
}
