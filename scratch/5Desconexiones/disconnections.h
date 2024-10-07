#ifndef disconnections_H
#define disconnections_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;

extern NodeContainer nodes;                   // Container for client nodes
extern std::set<uint32_t> disconnectedNodes;  // Set to store disconnected nodes
extern std::string nodesDisconnectedString;   // String to store disconnected nodes in order

// Function to disconnect a random node
void DisconnectRandomNode();

// Function for getting all reachable nodes from startNode
std::set<uint32_t> GetReachableNodes(Ptr<Node> startNode, const std::vector<std::pair<uint32_t, uint32_t>>& links, std::set<uint32_t> disconnectedNodes);

#endif