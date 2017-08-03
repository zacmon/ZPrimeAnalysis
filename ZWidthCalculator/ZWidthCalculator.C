

#include <cmath>
#include <iostream>

double PI = 3.141592653589793238462643383279502884197;

void ZWidthCalculator() {
    std::vector<double> massQuark = {4.8e-3, 2.3e-3, 9.5e-2, 1.275e0, 4.66e0, 1.7321e2};
    std::vector<double> couplings = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    std::vector<double> massZPrime = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 250};
    
    std::vector<std::vector<double>> widthSM(massZPrime.size(), std::vector<double>(couplings.size()));

    for (unsigned int i(0); i < massZPrime.size(); ++i) {
	for (unsigned int j(0); j < massQuark.size(); ++j) {
	    if (massQuark[j] > massZPrime[i] /2) {
		continue;
	    }
	    for (unsigned int k(0); k < couplings.size(); ++k) {
		widthSM[i][k] += 3 * massZPrime[i] * pow(couplings[k], 2) / (12 * PI) * pow(1 - pow(2 * massQuark[j], 2) / pow(massZPrime[i], 2), 3/2);
	    }
	}
    }
    
    for (unsigned int i(0); i < massZPrime.size(); ++i) {
	for (unsigned int j(0); j < couplings.size(); ++j) {
	    std::cout << massZPrime[i] << "GeV, " << couplings[j] << " width= " << widthSM[i][j] << std::endl;
	}
	std::cout << "\n";
    }
}
