#ifndef meassureFunctions_H
#define meassureFunctions_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/error-model.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

#include <iostream>
#include <fstream>


using namespace ns3;


// Variables for statistics
extern uint64_t totalReceivedBytes;       // Total received bytes
extern uint64_t totalTransmittedPackets;  // Total transmitted packets
extern uint64_t totalReceivedPackets;     // Total received packets

extern uint32_t receivedPacketCount;    // Number of packets received
extern uint32_t transmitedPacketCount;  // Number of packets transmitted


// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures(int nodesDisconnected, std::ostream& output);

// Function to handle TCP packet events
void nodeTxTrace(Ptr<const Packet> packet);

// Function to handle reception at each sink
void nodeRxTrace(Ptr<const Packet> packet);
// Function for dropping packets at disconnected sinks
void disconnectednodeRxTrace(Ptr<const Packet> packet);

#endif
