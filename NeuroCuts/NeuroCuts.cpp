#include "NeuroCuts.h"
#include <bitset>

int NeuroCuts::ClassifyAPacket(const Packet &packet) {
    uint64_t Query = 0;
    return ClassifyAPacket(packet, Query);
}

int NeuroCuts::ClassifyAPacket(const Packet &packet, uint64_t &Query) {
    int matchPri = -1;
    if(nodeSet[0]) matchPri = trieLookup(packet, nodeSet[0], 3, Query);
    if(nodeSet[1] && maxPri[1] > matchPri) matchPri = trieLookup(packet, nodeSet[1], 1, Query);
    if(nodeSet[2] && maxPri[2] > matchPri) matchPri = trieLookup(packet, nodeSet[2], 2, Query);
    if(HSbig && maxPri[3] > matchPri) matchPri = HSbig->ClassifyAPacket(packet, Query);
    // Assuming there is a similar QueryUpdate function for NeuroCuts
    // QueryUpdate(Query);
    return matchPri;
}

// int NeuroCuts::trieLookup(const Packet &packet, NeuroCutsNode *root, int speedUpFlag, uint64_t &Query) {
//     int matchPri = -1;
//     NeuroCutsNode* node = root;
//     unsigned int numbit = 32;
//     unsigned int cchild;

//     if(!node) {
//         return -1;
//     }
//     if(node->nodeType == TSS) {
//         // Assuming NeuroCutsNode might have a different structure
//         matchPri = node->HSnode->ClassifyAPacket(packet, Query);
//     } else {
//         // Assuming NeuroCutsNode might have a list of rules similar to CutSplitNode
//         for(const Rule &rule : node->rules) {
//             Query++;
//             if(rule.MatchesPacket(packet)) {
//                 matchPri = rule.priority;
//                 break;
//             }
//         }
//     }

//     return matchPri;
// }

NeuroCutsNode* NeuroCuts::makeNode(const nlohmann::json &jNode) {
    NeuroCutsNode* newNode = new NeuroCutsNode;

    jNode.at("ranges").get_to(newNode->ranges);

    auto& rules = jNode["rules"];
    for (auto& rule : rules) {
        Rule newRule;
        newRule.priority = rule["priority"];
        for (size_t i = 0, j = 0; i < rule["ranges"].size(); i += 2, j++) {
            newRule.range[j][0] = rule["ranges"][i];     // start point
            newRule.range[j][1] = rule["ranges"][i + 1]; // end point
        }
        newNode->rules.push_back(newRule);
    }

    // Assuming "action" is either "partition" or not present/other values
    newNode->partition = jNode.contains("action") && jNode.at("action") == "partition";

    if (jNode.contains("children")) {
        for (auto& childJson : jNode["children"]) {
            NeuroCutsNode* childNode = makeNode(childJson);
            newNode->children.push_back(childNode);
        }
    }

    return newNode;
}

void NeuroCuts::loadFromJSON(const nlohmann::json &j) {
    NeuroCutsNode* root = makeNode(j["root"]);
    nodeSet.push_back(root);
}