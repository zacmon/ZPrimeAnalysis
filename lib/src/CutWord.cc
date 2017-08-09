#include "../interface/CutWord.h"

#include <cmath>

CutWord::CutWord() {
    cutWord = 0;
}

CutWord::CutWord(unsigned int cutWord) {
    this->cutWord = cutWord;
}

CutWord::~CutWord() {
}

bool CutWord::isCutFailed(unsigned int cut) {
    return (cutWord & 1 << cut) != 0;
}

void CutWord::setCutFailed(unsigned int cut) {
    cutWord = cutWord | 1 << cut;
}

void CutWord::setCutPassed(unsigned int cut) {
    cutWord = cutWord & ~(1 << cut);
}

void CutWord::flipCut(unsigned int cut) {
    cutWord = cutWord ^ 1 << cut;
}

unsigned int CutWord::maskCut(unsigned int cut) {
    return cutWord & ~(1 << cut);
}

unsigned int CutWord::maskCuts(std::vector<unsigned int> cuts) {
    unsigned int tmpCutWord = cutWord;
    for (auto &&cut : cuts) {
	tmpCutWord &= maskCut(cut);
    }
    return tmpCutWord;
}

bool CutWord::areCutsFailed(std::vector<unsigned int> cuts) {
    for (auto &&cut : cuts) {
        if (!isCutFailed(cut)) {
            return false;
        }
    }
    return true;
}

unsigned int CutWord::getCutWord() {
    return cutWord;
}
