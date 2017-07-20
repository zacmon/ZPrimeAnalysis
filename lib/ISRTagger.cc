#include "ISRTagger.h"

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
    double minDEta = 10;
    for (unsigned int i(0); i < jets.size(); ++i) {
        if (jet == jets[i]) {
            continue;
        }
        double dEta = getDeltaEta(jet, jets[i]);
        minDEta = std::min(dEta, minDEta);
    }
    return minDEta;
}

double ISRTagger::getMassRatio(Jet* jet) {
    return jet->P4().M() / jet->P4().Pt();
}

double ISRTagger::massRatio(Jet* iJet, Jet* jJet) {
    return std::max(getMassRatio(iJet), getMassRatio(jJet)) / std::min(getMassRatio(iJet), getMassRatio(jJet));
}
