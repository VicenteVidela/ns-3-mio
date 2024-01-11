#ifndef meassureFunctions_H
#define meassureFunctions_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/nix-vector-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/topology-read-module.h"
#include "ns3/mobility-module.h"
#include "ns3/error-model.h"
#include "ns3/random-variable-stream.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"

#include <iostream>
#include <numeric>
#include <fstream>

using namespace ns3;

// Control flags
extern bool print;      // Flag for printing information
extern bool animation;  // Flag for including animation in the simulation

// Simulation parameters
extern float stopSendingTime;           // Time to stop sending packets
extern float stopTime;                  // Simulation stop time
extern StringValue p2pDelay;            // Delay for point-to-point links
extern StringValue p2pDataRate;         // Data rate for point-to-point links
extern StringValue onoffDataRate;          // Data rate for OnOff applications
extern UintegerValue onoffPacketSize;   // Packet size for OnOff applications
extern StringValue CCAlgorithm;         // Congestion control algorithm
// Error rate for package loss
extern Ptr<RateErrorModel> em;          // Error model for point-to-point links
extern DoubleValue errorRate;           // Error rate for package loss

// Variables for statistics
extern uint64_t totalReceivedBytes;       // Total received bytes
extern uint64_t totalTransmittedPackets;  // Total transmitted packets
extern uint64_t totalReceivedPackets;     // Total received packets

// Flow monitor
extern FlowMonitorHelper flowmon;
extern Ptr<FlowMonitor> monitor;

// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures(bool detailedPrinting, std::ostream& output);

#endif
