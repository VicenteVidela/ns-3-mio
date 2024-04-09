#include "meassureFunctions.h"

std::map<uint32_t, Time> transmissionTimes; // Map to store transmission times for each packet
std::vector<double> latencies;         // Vector to store latencies for each packet
Time lastArrivalTime;                  // Time of last packet arrival
std::vector<double> jitters;           // Vector to store jitters for each packet

uint32_t receivedPacketCount = 0;
uint32_t transmitedPacketCount = 0;


// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures(int nodesDisconnected, std::ostream& output) {
  // Print the number of connected nodes
  output << "Nodes disconnected: " << nodesDisconnected << std::endl;

  output << "Total packets transmitted: " << transmitedPacketCount << std::endl;
  output << "Total packets received: " << receivedPacketCount << std::endl;

  // Print an empty line for better readability
  output << std::endl;
}

// Function to handle transmited packets
void nodeTxTrace(Ptr<const Packet> packet) {
  // Get the packet transmission time
  Time txTime = Simulator::Now();
  // Get the packet ID
  uint32_t packetId = packet->GetUid();
  // Store the transmission time
  transmissionTimes[packetId] = txTime;
  // Increment the number of transmitted packets
  transmitedPacketCount++;
}

// Function to handle reception at each sink
void nodeRxTrace(Ptr<const Packet> packet) {
  // Get the packet reception time
  Time rxTime = Simulator::Now();
  // Get the packet ID
  uint32_t packetId = packet->GetUid();
  // Get the packet transmission time
  Time txTime = transmissionTimes[packetId];
  // Calculate the latency
  double latency = (rxTime - txTime).GetSeconds();
  // Store the latency
  latencies.push_back(latency);
  // Calculate the jitter
  double jitter = (rxTime - lastArrivalTime).GetSeconds();
  // Store the jitter
  jitters.push_back(jitter);
  // Update the last arrival time
  lastArrivalTime = rxTime;
  // Increment the number of received packets
  receivedPacketCount++;
}

// Function for dropping packets at disconnected sinkss
void disconnectednodeRxTrace(Ptr<const Packet> packet) {
  return;
}