#include "NeuroCuts.h"
#include <bitset>

// int NeuroCuts::ClassifyAPacket(const Packet &packet) {
//     uint64_t Query = 0;
//     return ClassifyAPacket(packet, Query);
// }

// int NeuroCuts::ClassifyAPacket(const Packet &packet, uint64_t &Query) {
//     int matchPri = -1;
//     if(nodeSet[0]) matchPri = trieLookup(packet, nodeSet[0], 3, Query);
//     if(nodeSet[1] && maxPri[1] > matchPri) matchPri = trieLookup(packet, nodeSet[1], 1, Query);
//     if(nodeSet[2] && maxPri[2] > matchPri) matchPri = trieLookup(packet, nodeSet[2], 2, Query);
//     if(HSbig && maxPri[3] > matchPri) matchPri = HSbig->ClassifyAPacket(packet, Query);
//     // Assuming there is a similar QueryUpdate function for NeuroCuts
//     // QueryUpdate(Query);
//     return matchPri;
// }

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
    // std::cout << "Entering makeNode" << std::endl;
    
    NeuroCutsNode* newNode = new NeuroCutsNode;

    // std::cout << "Attempting to get 'ranges'" << std::endl;
    jNode.at("ranges").get_to(newNode->ranges);

    auto& rules = jNode["rules"];
    for (auto& rule : rules) {
        Rule newRule;
        newRule.priority = rule["priority"];
        
        // std::cout << "Processing rule with priority: " << newRule.priority << std::endl;
        
        for (size_t i = 0, j = 0; i < rule["ranges"].size(); i += 2, j++) {
            newRule.range[j][0] = rule["ranges"][i];     // start point
            newRule.range[j][1] = rule["ranges"][i + 1]; // end point
        }
        newNode->rules.push_back(newRule);
    }

    // std::cout << "Checking for 'action'" << std::endl;
    // Assuming "action" is either "partition" or not present/other values
    // if (jNode.contains("action")) {
    //     std::cout << jNode.at("action") << std::endl;
    // }
    newNode->partition = jNode.contains("action") && jNode.at("action") != nlohmann::json::value_t::null && jNode.at("action")[0] == "partition";

    if (jNode.contains("children")) {
        // std::cout << "Processing 'children'" << std::endl;
        for (auto& childJson : jNode["children"]) {
            NeuroCutsNode* childNode = makeNode(childJson);
            newNode->children.push_back(childNode);
        }
    }
    // } else {
    //     std::cout << "No 'children' to process" << std::endl;
    // }

    // std::cout << "Exiting makeNode" << std::endl;
    return newNode;
}

void NeuroCuts::loadFromJSON(const nlohmann::json &j) {
    NeuroCutsNode* root = makeNode(j["root"]);
    nodeSet.push_back(root);
}

bool NeuroCutsNode::contains(const Packet& packet) {
    // Assuming Packet is represented as an array or std::vector<int> of size 5
    // print the packet contents
    // std::cout << "Packet: ";
    // for (int i = 0; i < packet.size(); i++) {
    //     std::cout << packet[i] << " ";
    // }
    // std::cout << std::endl;
    // assert(packet.size() == 5);
    std::vector<int> dimensions = {
        static_cast<int>(packet[0]), static_cast<int>(packet[0] + 1),
        static_cast<int>(packet[1]), static_cast<int>(packet[1] + 1),
        static_cast<int>(packet[2]), static_cast<int>(packet[2] + 1),
        static_cast<int>(packet[3]), static_cast<int>(packet[3] + 1),
        static_cast<int>(packet[4]), static_cast<int>(packet[4] + 1)
    };
    return is_intersect_multi_dimension(dimensions);
}

Rule* NeuroCutsNode::match(const Packet& packet) {
    if (partition) {
        std::vector<Rule*> matches;
        for (NeuroCutsNode* child : children) {
            Rule* match = child->match(packet);
            if (match) {
                matches.push_back(match);
            }
        }
        if (!matches.empty()) {
            // Sorting by the index of the rule in the `rules` vector
            std::sort(matches.begin(), matches.end(), [&](const Rule* a, const Rule* b) {
                return std::find(rules.begin(), rules.end(), *a) < std::find(rules.begin(), rules.end(), *b);
            });
            return matches.front();
        }
        return nullptr;
    } else if (!children.empty()) {
        for (NeuroCutsNode* child : children) {
            if (child->contains(packet)) {
                return child->match(packet);
            }
        }
        return nullptr;
    } else {
        for (Rule& rule : rules) {
            if (rule.matches(packet)) {  // Assuming 'matches' is a method in Rule class
                return &rule;
            }
        }
        return nullptr;
    }
}

bool NeuroCutsNode::is_intersect_multi_dimension(const std::vector<int>& ranges) {
    for (int i = 0; i < 5; i++) {
        if (ranges[i * 2] >= this->ranges[i * 2 + 1] || ranges[i * 2 + 1] <= this->ranges[i * 2]) {
            return false;
        }
    }
    return true;
}