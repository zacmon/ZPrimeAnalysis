#include "../interface/ISRTagger.h"

#include <algorithm>
#include <cmath>

#include <TLorentzVector.h>

ISRTagger::ISRTagger() {
}

ISRTagger::~ISRTagger() {
}

double ISRTagger::pTRatio(Jet* iJet, Jet* jJet) {
    return std::max(iJet->P4().Pt(), jJet->P4().Pt()) / std::min(iJet->P4().Pt(), jJet->P4().Pt());
}

double ISRTagger::getDeltaEta(Jet* iJet, Jet* jJet) {
    return std::abs(iJet->P4().Eta() - jJet->P4().Eta());
}

double ISRTagger::deltaEta(Jet* jet, std::vector<Jet*> jets) {
    if (jets.size() < 2) {
	return 0;
    }
    double minDEta = 10;
    for (auto &&jJet : jets) {
        if (jet == jJet) {
            continue;
        }
        double dEta = getDeltaEta(jet, jJet);
        minDEta = std::min(dEta, minDEta);
    }
    return minDEta;
}

double ISRTagger::getDeltaPhi(Jet* iJet, Jet* jJet) {
    return std::abs(iJet->P4().DeltaPhi(jJet->P4()));
}

double ISRTagger::deltaPhi(Jet* jet, std::vector<Jet*> jets) {
    if (jets.size() < 2) {
	return 0;
    }
    double minDPhi = 10;
    for (auto &&jJet : jets) {
	if (jet == jJet) {
	    continue;
	}
	double dPhi = getDeltaPhi(jet, jJet);
	minDPhi = std::min(dPhi, minDPhi);
    }
    return minDPhi;
}

double ISRTagger::getDeltaR(Jet* iJet, Jet* jJet) {
    return iJet->P4().DeltaR(jJet->P4());
}

double ISRTagger::deltaR(Jet* jet, std::vector<Jet*> jets) {
    if (jets.size() < 2) {
	return 0;
    }
    double minDR = 10;
    for (auto &&jJet : jets) {
	if (jet == jJet) {
	    continue;
	}
	double dR = getDeltaR(jet, jJet);
	minDR = std::min(dR, minDR);
    }
    return minDR;
}

double ISRTagger::getMassRatio(Jet* jet) {
    return jet->P4().M() / jet->P4().Pt();
}

double ISRTagger::massRatio(Jet* iJet, Jet* jJet) {
    return std::max(getMassRatio(iJet), getMassRatio(jJet)) / std::min(getMassRatio(iJet), getMassRatio(jJet));
}

int ISRTagger::getNumTracks(Jet* jet, std::vector<Track*> tracks, double jetRadius) {
    int numTracks = 0;
    for (auto && track : tracks) {
	if (jet->P4().DeltaR(track->P4()) < jetRadius) {
	    ++numTracks;
	}
    }
    return numTracks;
}    
