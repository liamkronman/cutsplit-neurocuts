#ifndef CUTTSSV1_2_NEUROCUTS_H
#define CUTTSSV1_2_NEUROCUTS_H

#include <utility>

#include "./../ElementaryClasses.h"
#include "./../HyperSplit/HyperSplit.h"
using namespace std;

struct NeuroCutsNode {
    bool isLeaf;
    int nrules;
    int depth;
    int nodeType; // (Cuts, Linear, TSS)

    vector<Rule> rules;
    vector<int> ncuts;
    vector<vector<unsigned int> > field;
    HyperSplit *HSnode;

    vector<NeuroCutsNode*> children;
    explicit NeuroCutsNode(const vector<Rule> &r, vector<vector<unsigned int> > f, int level = 0, bool isleaf = true, NodeType flag = Cuts) {
        depth = level;
        isLeaf = isleaf;
        nodeType = flag;
        field = std::move(f);
        ncuts.resize(MAXDIMENSIONS, 1);
        rules = r;
        nrules = int(r.size());
        HSnode = nullptr;

//        field[LowDim].resize(MAXDIMENSIONS);
//        field[HighDim].resize(MAXDIMENSIONS);
    }
};



#endif