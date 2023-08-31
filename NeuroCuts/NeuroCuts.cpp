#include "NeuroCuts.h"

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

int NeuroCuts::trieLookup(const Packet &packet, NeuroCutsNode *root, int speedUpFlag, uint64_t &Query) {
    int matchPri = -1;
    NeuroCutsNode* node = root;
    unsigned int numbit = 32;
    unsigned int cchild;

    // ... (The same logic for looking up a trie, assuming NeuroCuts has a similar structure)

    if(!node) {
        return -1;
    }
    if(node->nodeType == TSS) {
        // Assuming NeuroCutsNode might have a different structure
        matchPri = node->HSnode->ClassifyAPacket(packet, Query);
    } else {
        // Assuming NeuroCutsNode might have a list of rules similar to CutSplitNode
        for(const Rule &rule : node->rules) {
            Query++;
            if(rule.MatchesPacket(packet)) {
                matchPri = rule.priority;
                break;
            }
        }
    }

    return matchPri;
}

void NeuroCuts::loadFromJSON(const nlohmann::json &j) {
    // Assuming the JSON structure contains a list of nodes in the "nodes" field.
    for (const auto& nodeObj : j.at("nodes")) {
        NeuroCutsNode* newNode = new NeuroCutsNode;

        nodeObj.at("isLeaf").get_to(newNode->isLeaf);
        nodeObj.at("nrules").get_to(newNode->nrules);
        nodeObj.at("depth").get_to(newNode->depth);
        nodeObj.at("nodeType").get_to(newNode->nodeType);
        nodeObj.at("ncuts").get_to(newNode->ncuts);

        for (const auto& ruleObj : nodeObj.at("rules")) {
            Rule rule;

            ruleObj.at("dim").get_to(rule.dim);
            ruleObj.at("priority").get_to(rule.priority);
            ruleObj.at("id").get_to(rule.id);
            ruleObj.at("tag").get_to(rule.tag);
            ruleObj.at("markedDelete").get_to(rule.markedDelete);
            ruleObj.at("prefix_length").get_to(rule.prefix_length);

            for (int i = 0; i < rule.dim; i++) {
                std::array<Point, 2> arr = {ruleObj["range"][i][0], ruleObj["range"][i][1]};
                rule.range.push_back(arr);
            }

            newNode->rules.push_back(rule);
        }

        for (const auto& fieldObj : nodeObj.at("field")) {
            newNode->field.push_back(fieldObj.get<std::vector<unsigned int>>());
        }

        nodeSet.push_back(newNode);
    }

    // Note: Make sure to free the memory of the nodes (using delete) when they're no longer needed.
}