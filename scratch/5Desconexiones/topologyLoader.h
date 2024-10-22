#ifndef topologyLoader_H
#define topologyLoader_H

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

// Function to load the topology from a csv file with format "lX,lY"
std::vector<std::pair<uint32_t, uint32_t>> LoadTopology(const std::string& input);

// Function to load the providers from a txt file
std::vector<uint32_t> LoadProviders(const std::string& input);

// Function for getting all reachable nodes from startNode
std::set<uint32_t> GetReachableNodes(Ptr<Node> startNode, const std::vector<std::pair<uint32_t, uint32_t>>& links, std::set<uint32_t> disconnectedNodes);


#endif