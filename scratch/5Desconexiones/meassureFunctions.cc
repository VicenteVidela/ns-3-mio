#include "meassureFunctions.h"

using namespace ns3;

std::map<uint32_t, Time> transmissionTimes; // Map to store transmission times for each packet
std::vector<double> latencies;         // Vector to store latencies for each packet
Time lastArrivalTime;                  // Time of last packet arrival
std::vector<double> jitters;           // Vector to store jitters for each packet

// Flow monitor
FlowMonitorHelper flowmon;
Ptr<FlowMonitor> monitor;

// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures(bool detailedPrinting, std::ostream& output) {
  // Print the current simulation time
  output << "Seconds: " << Simulator::Now().GetSeconds() << std::endl;
  // Variable Declarations
  double totalDelay = 0;                  // Accumulator for total delay of all flows
  uint64_t totalPackets = 0;              // Total received packets across all flows
  uint64_t totalLostPackets = 0;          // Total lost packets across all flows
  uint64_t totalTransmittedPackets = 0;   // Total transmitted packets across all flows
  double totalReceivedBytes = 0;          // Total received bytes across all flows
  double totalDuration = 0;                // Accumulator for total duration of all flows
  double totalJitter = 0;                  // Accumulator for total jitter of all flows

  // Check for lost packets and get flow statistics
  monitor->CheckForLostPackets();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowmon.GetClassifier());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats();

  // Iterate through flow statistics and accumulate values
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin(); i != stats.end(); ++i) {
    totalReceivedBytes += i->second.rxBytes;
    totalDuration += i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds();
    totalDelay += i->second.delaySum.GetSeconds();
    totalPackets += i->second.rxPackets;
    totalLostPackets += i->second.lostPackets;
    totalTransmittedPackets += i->second.txPackets;
    totalJitter += i->second.jitterSum.GetSeconds();
  }

  // Calculate and print average throughput
  double averageThroughput = totalReceivedBytes * 8.0 / totalDuration;
  output << "Average Throughput: " << averageThroughput << " bps" << std::endl;

  // Calculate and print average packet delivery ratio and percentage
  double averagePacketDeliveryRatio = static_cast<double>(totalPackets) / static_cast<double>(totalTransmittedPackets);
  double averagePacketDeliveryPercentage = averagePacketDeliveryRatio * 100;
  output << "Average Packet Delivery Ratio: " << averagePacketDeliveryPercentage << "%" << std::endl;

  // Calculate and print average packet loss rate and percentage
  double averagePacketLossRate = static_cast<double>(totalLostPackets) / static_cast<double>(totalTransmittedPackets);
  double averagePacketLossPercentage = averagePacketLossRate * 100;
  output << "Average Packet Loss Rate: " << averagePacketLossPercentage << "%" << std::endl;

  // Check if there are received packets and print average delay if applicable
  if (totalPackets > 0) {
    double averageDelayMs = (totalDelay / totalPackets) * 1000;
    output << "Average delay: " << averageDelayMs << " ms" << std::endl;
  } else {
    output << "Average delay: 0 ms" << std::endl;
  }

  // Calculate and print average jitter
  double averageJitterMs = (totalJitter / stats.size()) * 1000;
  output << "Average Jitter: " << averageJitterMs << " ms" << std::endl;

  // Print an empty line for better readability
  output << std::endl;
  // Reset flowmonitor
  monitor->ResetAllStats();
}