#include "meassureFunctions.h"

using namespace ns3;

std::map<uint32_t, Time> transmissionTimes; // Map to store transmission times for each packet
std::vector<double> latencies;         // Vector to store latencies for each packet
Time lastArrivalTime;                  // Time of last packet arrival
std::vector<double> jitters;           // Vector to store jitters for each packet

// Callback function to process received packets and update statistics
void SinkRx(Ptr<const Packet> p, const Address& ad)
{
  Ipv4Header ipv4;
  p->PeekHeader(ipv4);
  // Update total received bytes and packets
  totalReceivedBytes += p->GetSize();
  totalReceivedPackets++;

  // Calculate latency
  Time txTime = transmissionTimes[p->GetUid()];
  Time latency = Simulator::Now() - txTime;
  latencies.push_back(latency.GetSeconds());

  // Calculate jitter
  if (lastArrivalTime != Time(0)) {
    Time interArrivalTime = Simulator::Now() - lastArrivalTime;
    double jitter = std::abs(interArrivalTime.GetSeconds() - latency.GetSeconds());
    jitters.push_back(jitter);
  }
  lastArrivalTime = Simulator::Now();
}

// Callback function when packets are transmitted
void OnOffTx(Ptr<const Packet> packet) {
  totalTransmittedPackets++;    // Update total transmitted packets
  transmissionTimes[packet->GetUid()] = Simulator::Now();   // Store transmission time for the packet
}

// Function to print throughput and packet statistics at the end of the simulation
void PrintMeassures(bool detailedPrinting) {
  std::cout << std::endl;
  std::cout << "Seconds: " << Simulator::Now().GetSeconds() << std::endl;
  // Print throughput
  if (detailedPrinting) std::cout << "Total received bytes: " << totalReceivedBytes << std::endl;
  double throughput = static_cast<double>(totalReceivedBytes) / (Simulator::Now().GetSeconds());
  std::cout << "Throughput: " << throughput << " bytes/second" << std::endl;
  if (detailedPrinting) std::cout << std::endl;

  // Print packet statistics
  if (detailedPrinting) {
    std::cout << "Total transmitted packets: " << totalTransmittedPackets << std::endl;
    std::cout << "Total received packets: " << totalReceivedPackets << std::endl;
  }
  // Calculate and print packet loss percentage
  double packetLossPercentage = ((totalTransmittedPackets - totalReceivedPackets) / static_cast<double>(totalTransmittedPackets)) * 100.0;
  std::cout << "Packet Loss Percentage: " << packetLossPercentage << "%" << std::endl;
  if (detailedPrinting) std::cout << std::endl;

  // Print latency statistics
  double latency_sum = std::accumulate(latencies.begin(), latencies.end(), 0.0);
  double latency_mean = latency_sum / latencies.size();
  double latency_sq_sum = std::inner_product(latencies.begin(), latencies.end(), latencies.begin(), 0.0);
  double latency_std_dev = std::sqrt(latency_sq_sum / latencies.size() - latency_mean * latency_mean);
  double max_latency = *std::max_element(latencies.begin(), latencies.end());
  double min_latency = *std::min_element(latencies.begin(), latencies.end());
  std::cout << "Mean latency: " << latency_mean << " seconds" << std::endl;
  if (detailedPrinting) {
    std::cout << "Standard deviation of latency: " << latency_std_dev << " seconds" << std::endl;
    std::cout << "Maximum latency: " << max_latency << " seconds" << std::endl;
    std::cout << "Minimum latency: " << min_latency << " seconds" << std::endl;
    std::cout << std::endl;
  }

  // Print jitter statistics
  double jitter_sum = std::accumulate(jitters.begin(), jitters.end(), 0.0);
  double jitter_mean = jitter_sum / jitters.size();
  double jitter_sq_sum = std::inner_product(jitters.begin(), jitters.end(), jitters.begin(), 0.0);
  double jitter_std_dev = std::sqrt(jitter_sq_sum / jitters.size() - jitter_mean * jitter_mean);
  double max_jitter = *std::max_element(jitters.begin(), jitters.end());
  double min_jitter = *std::min_element(jitters.begin(), jitters.end());

  std::cout << "Mean jitter: " << jitter_mean << " seconds" << std::endl;
  if (detailedPrinting) {
    std::cout << "Standard deviation of jitter: " << jitter_std_dev << " seconds" << std::endl;
    std::cout << "Maximum jitter: " << max_jitter << " seconds" << std::endl;
    std::cout << "Minimum jitter: " << min_jitter << " seconds" << std::endl;
    std::cout << std::endl;
  }
}

// Function to reset meassures
void ResetMeassures() {
  // Reset byte counters
  totalReceivedBytes = 0;
  totalTransmittedPackets = 0;
  totalReceivedPackets = 0;

  // Reset latency and jitter vectors
  latencies.clear();
  jitters.clear();

  // Reset transmission times map
  transmissionTimes.clear();
}