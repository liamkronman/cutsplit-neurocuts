#include "NeuroCuts.h"
using namespace std;

NeuroCuts::NeuroCuts

int NeuroCuts::ClassifyAPacket(const Packet &packet) {
    if(!root) {
        return -1;
    }
    NeuroCutsNode *node = root;
    int Query = 0;
    int matchPri = -1;

    while(!node->isLeaf) {
        Query ++;
        if(packet[node->d2s] <= node->thresh) {
            node = node->children[0];
        } else {
            node = node->children[1];
        }
    }
    for(auto & rule : node->ruleset) {
        Query ++;
        if(rule.MatchesPacket(packet)){
            matchPri = rule.priority;
            break;
        }
    }
    QueryUpdate(Query);
    return matchPri;
}