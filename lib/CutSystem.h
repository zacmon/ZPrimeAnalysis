#ifndef ZPRIMEANALYSIS_LIB_CUTSYSTEM
#define ZPRIMEANALYSIS_LIB_CUTSYSTEM

#include <vector>

class CutSystem{
  public:
    unsigned int cuts;
    
    CutSystem();
    CutSystem(unsigned int inCuts);
    ~CutSystem();

    int maskCut(unsigned int cutType);
    int maskCuts(std::vector<unsigned int> cutTypes);
    bool isCutFailed(unsigned int cutType);
    bool areCutsFailed(std::vector<unsigned int> cutTypes);

    void setCutFailed(unsigned int cutType);
    void setCutPassed(unsigned int cutType);
    void flipCut(unsigned int cutType);

    int getCuts();
};

#endif
