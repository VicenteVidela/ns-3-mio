#include "meassureFunctions.h"

std::vector<double> latencies;              // Vector to store latencies for each packet
Time lastArrivalTime;                       // Time of last packet arrival
std::vector<double> jitters;                // Vector to store jitters for each packet

uint32_t receivedPacketCount = 0;           // Number of packets received
uint32_t transmitedPacketCount = 0;         // Number of packets transmitted
double totalBandwidth = 0.0;                // Total maximum bandwidth of the network
uint32_t totalBytesReceived = 0;            // Total bytes received
uint32_t totalNodes = 0;                   // Total number of nodes in the network

// std::map<uint32_t, Time> enqueueTimes;      // Map to store enqueue times for each packet
// std::vector<double> queueDelays;            // Vector to store queue delays for each packet

std::map<uint32_t, Time> sendTimes;         // Map to store send times for each packet


// Function to print statistics at the end of the simulation
void PrintMeasures(std::set<uint32_t> nodesDisconnected, std::ostream& output, float stopTime, std::string nodesDisconnectedString) {
  // Print the disconnected nodes as a list
  output << "Nodes disconnected: " << nodesDisconnectedString << std::endl;

  // Print the number of disconnected nodes
  output << "Number of disconnected nodes: " << nodesDisconnected.size() << std::endl;

  // Print max available bandwidth in Kbps
  output << "Max available bandwidth: " << totalBandwidth / 1e3 << " Kbps" << std::endl;
  // Calculate and print average throughput in Kbps
  double averageThroughput = static_cast<double>(totalBytesReceived * 8) / (stopTime * 1e3);
  output << "Average throughput: " << averageThroughput << " Kbps" << std::endl;

  // Calculate and print througput to bandwidth ratio
  double throughputToBandwidthRatio = averageThroughput / (totalBandwidth / 1e3);
  output << "Throughput to bandwidth ratio: " << throughputToBandwidthRatio << std::endl;

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

  // Calculate and print average queue delay
  // double totalQueueDelay = std::accumulate(queueDelays.begin(), queueDelays.end(), 0.0);
  // double averageQueueDelay = totalQueueDelay * 1000/ queueDelays.size();
  // output << "Average queue delay: " << averageQueueDelay << " ms" << std::endl;

  // Print an empty line for better readability
  output << std::endl;
}


/**
 * Physical layer functions
*/
// Function to handle transmited packets at the physical layer
void nodeTxTrace(std::string context, Ptr<const Packet> packet) {
  transmitedPacketCount++;
}

// Function to handle reception at each sink at the physical layer
void nodeRxTrace(Ptr<const Packet> packet) {
  receivedPacketCount++;

  // Calculate and store the size of the received packet
  uint32_t packetSize = packet->GetSize(); // Size of the packet in bytes
  totalBytesReceived += packetSize;
}


/**
 * Application layer functions
*/
// Function to handle transmited packets at the application layer
void applicationTxTrace(Ptr<const Packet> packet) {
  // Get the packet ID
  UniqueIdentifierHeader uidHeader;
  packet->PeekHeader(uidHeader);
  uint32_t packetId = uidHeader.GetId();

  // Store the send time
  sendTimes[packetId] = Simulator::Now();
}

// Function to handle reception at the application layer
void applicationRxTrace(Ptr<const Packet> packet, const Address &from) {
  // Get the packet ID
  UniqueIdentifierHeader uidHeader;
  packet->PeekHeader(uidHeader);
  uint32_t packetId = uidHeader.GetId();

  // Get the packet transmission time
  Time txTime = sendTimes[packetId];
  // Get the packet reception time
  Time rxTime = Simulator::Now();
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
}



/**
 * Queue functions
*/
// Function for tracing packets entering queue
// void nodeQueueEnqueueTrace(Ptr<const Packet> packet) {
//   Ptr<Packet> mutablePacket = packet->Copy();

//   // Remove the Ethernet header
//   // EthernetHeader ethHeader;
//   // mutablePacket->RemoveHeader(ethHeader);

//   // Remove the sequence number header
//   SeqTsSizeHeader header;
//   mutablePacket->RemoveHeader(header);

//   // Get the packet ID
//   UniqueIdentifierHeader uidHeader;
//   mutablePacket->PeekHeader(uidHeader);

//   // Store the enqueue time
//   enqueueTimes[uidHeader.GetId()] = Simulator::Now();
// }

// // Function for tracing packets leaving queue
// void nodeQueueDequeueTrace(Ptr<const Packet> packet) {
//   Ptr<Packet> mutablePacket = packet->Copy();

//   // Remove the Ethernet header
//   // EthernetHeader ethHeader;
//   // mutablePacket->RemoveHeader(ethHeader);

//   // Remove the sequence number header
//   SeqTsSizeHeader header;
//   mutablePacket->RemoveHeader(header);

//   // Get the packet ID
//   UniqueIdentifierHeader uidHeader;
//   mutablePacket->PeekHeader(uidHeader);

//   uint32_t uniqueId = uidHeader.GetId();
//   // Get the packet enqueue time
//   Time enqueueTime = enqueueTimes[uniqueId];
//   // Get the packet dequeue time
//   Time dequeueTime = Simulator::Now();
//   // Calculate the queue delay
//   double queueDelay = (dequeueTime - enqueueTime).GetSeconds();
//   // Store the queue delay
//   queueDelays.push_back(queueDelay);
// }

// // Function for tracing queue length
// void QueueLengthTrace(std::string context, uint32_t oldValue, uint32_t newValue) {
//   // std::cout << "Queue length changed from " << oldValue << " to " << newValue << std::endl;
//   return;
// }

