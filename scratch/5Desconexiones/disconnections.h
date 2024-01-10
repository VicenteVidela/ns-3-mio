#ifndef disconnections_H
#define disconnections_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;

// Function to disconnect a random node
void DisconnectRandomNode(NodeContainer& nodes);

#endif