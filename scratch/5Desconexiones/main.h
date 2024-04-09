#ifndef main_H
#define main_H

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

#include "meassureFunctions.h"
#include "disconnections.h"

using namespace ns3;

// Simulation parameters
extern float stopSendingTime;                               // Time to stop sending packets
extern float stopTime;                                      // Simulation stop time
extern int timeInterval;                                    // Time interval for printing statistics
extern int timeIntervalInit;                                // Initial time for printing statistics
extern ns3::StringValue p2pDelay;                           // Delay for point-to-point links
extern ns3::StringValue p2pDataRate;                        // Data rate for point-to-point links
extern ns3::StringValue onoffDataRate;                      // Data rate for OnOff applications
extern ns3::UintegerValue onoffPacketSize;                  // Packet size for OnOff applications
extern ns3::StringValue CCAlgorithm;                        // Congestion control algorithm
extern int nodesToDisconnect;                               // Number of nodes to disconnect
// Error rate for package loss
extern ns3::Ptr<ns3::RateErrorModel> em;                    // Error model for point-to-point links
extern ns3::DoubleValue errorRate;                          // Error rate for package loss

// Directory for topology files
std::string topologyDirectory  = "scratch/topologies/";     // Directory for topology files

// Directory for data files
std::string dataDirectory = "data/";                        // Directory for data files
extern std::string dataFile;                                // Data file name
std::ofstream outputFile;                                   // Output file stream

// Default values for command line parameters
extern std::string format;                                  // Format for command line parameters
extern std::string input;                                   // Input file name

#endif