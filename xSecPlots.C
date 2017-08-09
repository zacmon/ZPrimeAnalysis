#include <vector>
#include <iostream>

#include <TLegend.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TAxis.h>
void xSecPlots() {
    std::vector<int> mass = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 250};
    double couplings[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    std::vector<std::vector<double>> xSec;
    std::cout << "1";
    xSec.push_back({0.02358, 0.09811,0.2474, 0.523, 0.9929, 1.746, 2.868, 4.42, 6.565, 9.315});
    xSec.push_back({0.1178, 0.4697, 1.055, 1.865, 2.904, 4.167, 5.667, 7.395, 9.374, 11.63});
    xSec.push_back({0.08547, 0.342, 0.7678, 1.359, 2.102, 3.009, 4.064, 5.285, 6.671, 8.234});
    xSec.push_back({0.06216, 0.2488, 0.5586, 0.9848, 1.531, 2.194, 2.972, 3.868, 4.892, 6.062});
    xSec.push_back({0.04744, 0.1899, 0.4268, 0.7558, 1.175, 1.685, 2.292, 2.996, 3.799, 4.716});
    xSec.push_back({0.03793, 0.1519, 0.3412, 0.6047, 0.9438, 1.355, 1.852, 2.426, 3.094, 3.829});
    xSec.push_back({0.03149, 0.126, 0.2837, 0.5034, 0.7858, 1.136, 1.553, 2.042, 2.607, 3.24});
    xSec.push_back({0.02698, 0.1079, 0.2428, 0.4326, 0.676, 0.9806, 1.345, 1.771, 2.265, 2.824});
    xSec.push_back({});
    xSec.push_back({0.02138, 0.08544, 0.1926, 0.3438, 0.5419, 0.7869, 1.081, 1.427, 1.829, 2.273});
    xSec.push_back({0.01185, 0.04729, 0.1059, 0.1861, 0.2841, 0.4001, 0.5307, 0.6699, 0.8243, 0.9827});
    std::cout << "2" << std::endl;
    TMultiGraph* mg = new TMultiGraph();
    TLegend* legend = new TLegend(0.85, 0.85, 0.70, 0.75);
    for (unsigned int i = 0; i < mass.size(); ++i) {
	if (xSec[i].size() != 10) continue;
	double xSecArray[10];
	std::copy(xSec[i].begin(), xSec[i].end(), xSecArray);
	TGraph* g = new TGraph(xSec[i].size(), couplings, xSecArray);
	g->SetMarkerStyle(8);
        g->SetMarkerSize(2);
        g->SetLineWidth(3);
	g->SetLineStyle(10);
	if (i < 9) {
            g->SetMarkerColor(i+1);
        }
        else {
            std::cout << "here" << std::endl;
            g->SetMarkerColor(i+20);
        }
	legend->AddEntry(g, Form("%s %d", "m_{Z'}=", mass[i]), "p");
	mg->Add(g);
    }

    TCanvas* c = new TCanvas("c", "c");
    c->SetLogy();
    mg->Draw("APL");
    mg->GetXaxis()->SetTitle("g_{SM}");
    mg->GetYaxis()->SetTitle("#sigma [pb]");
    legend->Draw();
    gPad->Modified();
}
