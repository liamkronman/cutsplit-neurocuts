#ifndef CUTTSSV1_2_NEUROCUTS_H
#define CUTTSSV1_2_NEUROCUTS_H

#include <utility>

#include "./../ElementaryClasses.h"
#include "./../HyperSplit/HyperSplit.h"

#include <vector>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;

struct NeuroCutsNode {

    std::vector<int> ranges;
    std::vector<Rule> rules;
    std::vector<NeuroCutsNode*> children;
    bool partition;
    // 
};

class NeuroCuts {

public:
    // int ClassifyAPacket(const Packet &packet);
    // int ClassifyAPacket(const Packet &packet, uint64_t &Query);
    // int trieLookup(const Packet &packet, NeuroCutsNode *root, int speedUpFlag, uint64_t &Query);
    void loadFromJSON(const nlohmann::json &j);

private:
    // Example members from CutSplit, adjust as necessary for NeuroCuts
    vector<NeuroCutsNode*> nodeSet;
    // vector<int> maxPri;

    // Additional NeuroCuts specific members go here...
};

#endif