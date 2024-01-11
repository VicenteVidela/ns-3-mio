#include "meassureFunctions.h"

using namespace ns3;

// Callback function to process received packets and update statistics
void SinkRx(Ptr<const ns3::Packet> p, const ns3::Address& ad)
{
  ns3::Ipv4Header ipv4;
  p->PeekHeader(ipv4);

  // Update total received bytes and packets
  totalReceivedBytes += p->GetSize();
  totalReceivedPackets++;
}

void OnOffTx(Ptr<const ns3::Packet> packet)
{
  totalTransmittedPackets++;
}

// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures()
{
  // Print throughput
  std::cout << "Total received bytes: " << totalReceivedBytes << std::endl;
  double throughput = static_cast<double>(totalReceivedBytes) / (ns3::Simulator::Now().GetSeconds());
  std::cout << "Throughput: " << throughput << " bytes/second" << std::endl;

  // Print packet statistics
  std::cout << "Total transmitted packets: " << totalTransmittedPackets << std::endl;
  std::cout << "Total received packets: " << totalReceivedPackets << std::endl;

  // Calculate and print packet loss percentage
  double packetLossPercentage = ((totalTransmittedPackets - totalReceivedPackets) / static_cast<double>(totalTransmittedPackets)) * 100.0;
  std::cout << "Packet Loss Percentage: " << packetLossPercentage << "%" << std::endl;
}
