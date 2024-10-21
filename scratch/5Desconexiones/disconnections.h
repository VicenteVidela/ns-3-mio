#ifndef disconnections_H
#define disconnections_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/random-variable-stream.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <regex>

using namespace ns3;

extern NodeContainer nodes;                   // Container for client nodes
extern std::set<uint32_t> disconnectedNodes;  // Set to store disconnected nodes
extern std::string nodesDisconnectedString;   // String to store disconnected nodes in order

// Function to disconnect a random node
void DisconnectRandomNode();

// Function to disconnect a list of nodes
void DisconnectNodes(std::vector<uint32_t> nodesToDisconnect);

// Function for getting all reachable nodes from startNode
std::set<uint32_t> GetReachableNodes(Ptr<Node> startNode, const std::vector<std::pair<uint32_t, uint32_t>>& links, std::set<uint32_t> disconnectedNodes);

// Function to load the nodes from the file
std::vector<std::vector<uint32_t>> loadNodesToDisconnect(const std::string& filename);

#endif