#include "CutSystem.h"

#include <cmath>

CutSystem::CutSystem() {
    cuts = 0;
}

CutSystem::CutSystem(unsigned int inCuts) {
    cuts = inCuts;
}

CutSystem::~CutSystem() {
}

bool CutSystem::isCutFailed(unsigned int cutType) {
    return (cuts & 1 << cutType) != 0;
}

void CutSystem::setCutFailed(unsigned int cutType) {
    cuts = cuts | 1 << cutType;
}

void CutSystem::setCutPassed(unsigned int cutType) {
    cuts = cuts & ~(1 << cutType);
}

void CutSystem::flipCut(unsigned int cutType) {
    cuts = cuts ^ 1 << cutType;
}

int CutSystem::maskCut(unsigned int cutType) {
    return cuts & ~(1 << cutType);
}

int CutSystem::maskCuts(std::vector<unsigned int> cutTypes) {
    int tmpcuts = cuts;
    for (auto &&cutType : cutTypes) {
	tmpcuts &= maskCut(cutType);
    }
    return tmpcuts;
}

bool CutSystem::areCutsFailed(std::vector<unsigned int> cutTypes) {
    for (auto &&cutType : cutTypes) {
        if (!isCutFailed(cutType)) {
            return false;
        }
    }
    return true;
}

int CutSystem::getCuts() {
    return cuts;
}
