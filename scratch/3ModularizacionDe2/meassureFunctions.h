#ifndef meassureFunctions_H
#define meassureFunctions_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/nix-vector-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/topology-read-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/error-model.h"
#include "ns3/random-variable-stream.h"

using namespace ns3;

// Control flags
extern bool print;
extern bool animation;

// Simulation parameters
extern float stopTime;
extern StringValue p2pDelay;
extern StringValue p2pDataRate;
extern DataRate onoffDataRate;
extern UintegerValue onoffPacketSize;
extern Ptr<RateErrorModel> em;
extern DoubleValue errorRate;

// Variables for statistics
extern uint64_t totalReceivedBytes;
extern uint64_t totalTransmittedPackets;
extern uint64_t totalReceivedPackets;

// Directory for topology files
extern char topologyDirectory[];

// Callback function to process received packets and update statistics
void SinkRx(Ptr<const ns3::Packet> p, const ns3::Address& ad);

void OnOffTx(Ptr<const ns3::Packet> packet);

// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures();

#endif // MY_FUNCTIONS_H
