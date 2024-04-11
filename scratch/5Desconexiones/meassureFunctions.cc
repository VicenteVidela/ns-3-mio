#include "meassureFunctions.h"

std::map<uint32_t, Time> transmissionTimes; // Map to store transmission times for each packet
std::vector<double> latencies;         // Vector to store latencies for each packet
Time lastArrivalTime;                  // Time of last packet arrival
std::vector<double> jitters;           // Vector to store jitters for each packet

uint32_t receivedPacketCount = 0;
uint32_t transmitedPacketCount = 0;

// Function to print statistics at the end of the simulation
void PrintMeasures(int nodesDisconnected, std::ostream& output, float stopTime) {
  // Print the number of disconnected nodes
  output << "Nodes disconnected: " << nodesDisconnected << std::endl;

  // output << "Total packets transmitted: " << transmitedPacketCount << std::endl;
  // output << "Total packets received: " << receivedPacketCount << std::endl;

  // Calculate and print average throughput
  double averageThroughput = static_cast<double>(receivedPacketCount) / stopTime;
  output << "Average throughput: " << averageThroughput << " packets/sec" << std::endl;

  // Calculate and print packet loss percentage
  double packetLossPercentage = 100.0 * (1.0 - static_cast<double>(receivedPacketCount) / transmitedPacketCount);
  output << "Packet loss percentage: " << packetLossPercentage << "%" << std::endl;

  // Calculate and print average delay
  double totalDelay = std::accumulate(latencies.begin(), latencies.end(), 0.0);
  double averageDelay = totalDelay * 1000/ latencies.size();
  output << "Average delay: " << averageDelay << " ms" << std::endl;

  // Calculate and print average jitter
  double totalJitter = std::accumulate(jitters.begin(), jitters.end(), 0.0);
  double averageJitter = totalJitter * 1000 / jitters.size();
  output << "Average jitter: " << averageJitter << " ms" << std::endl;

  // Print an empty line for better readability
  output << std::endl;
}

// Function to handle transmited packets
void nodeTxTrace(Ptr<const Packet> packet) {
  // Get the packet transmission time
  // Time txTime = Simulator::Now();
  // // Get the packet ID
  // uint32_t packetId = packet->GetUid();
  // // Store the transmission time
  // transmissionTimes[packetId] = txTime;
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

// Function for dropping packets at disconnected sinks
void disconnectednodeRxTrace(Ptr<const Packet> packet) {
  return;
}

// Function for tracing queue length
void QueueLengthTrace(std::string context, uint32_t oldValue, uint32_t newValue) {
  // int32_t delta = newValue - oldValue;
  // std::cout << "Queue " << context << " length changed from " << oldValue << " to " << newValue << std::endl;
  return;
}

void nodeQueueEnqueueTrace(Ptr<const Packet> packet) {
  // Get the packet transmission time
  Time txTime = Simulator::Now();
  // Get the packet ID
  uint32_t packetId = packet->GetUid();
  // Store the transmission time
  transmissionTimes[packetId] = txTime;
}
