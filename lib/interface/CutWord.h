#ifndef ZPRIMEANALYSIS_LIB_CUTWORD
#define ZPRIMEANALYSIS_LIB_CUTWORD

#include <vector>

class CutWord {
  public:
    unsigned int cutWord;
    
    CutWord();
    CutWord(unsigned int cutWord);
    ~CutWord();

    unsigned int maskCut(unsigned int cutType);
    unsigned int maskCuts(std::vector<unsigned int> cutTypes);
    bool isCutFailed(unsigned int cutType);
    bool areCutsFailed(std::vector<unsigned int> cutTypes);

    void setCutFailed(unsigned int cutType);
    void setCutPassed(unsigned int cutType);
    void flipCut(unsigned int cutType);

    unsigned int getCutWord();
};

#endif
