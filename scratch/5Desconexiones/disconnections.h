#ifndef disconnections_H
#define disconnections_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/random-variable-stream.h"
#include "PacketSinkDisconnected.h"

using namespace ns3;

extern NodeContainer clientNodes;             // Container for client nodes
extern std::set<uint32_t> disconnectedNodes;  // Set to store disconnected nodes

// Function to disconnect a random node
void DisconnectRandomNode(Ptr<PacketSink> sinkDisconnected);
// Define a callback function for packet reception
bool DropPacket(Ptr<const Packet> packet, const Address& from);

#endif