#include "SinkRxWithNodeId.h"

SinkRxWithNodeId::SinkRxWithNodeId(uint32_t nodeId) : m_nodeId(nodeId) {}

void SinkRxWithNodeId::operator()(ns3::Ptr<const ns3::Packet> packet, const ns3::Address& addr) {
  if (disconnectedNodes.find(m_nodeId) != disconnectedNodes.end()){
    std::cout << "Packet dropped at " << m_nodeId << std::endl;
  }
  else {
    std::cout << "Packet received at " << m_nodeId << std::endl;
  }
}
