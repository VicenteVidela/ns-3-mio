#include "uniqueIdHeader.h"

uint32_t g_nextUniqueId = 0;    // UniqueId for packets


/**
 * UniqueIdentifierHeader class
 */
UniqueIdentifierHeader::UniqueIdentifierHeader() : m_uniqueId(0) {}

// Set the unique identifier
void UniqueIdentifierHeader::SetId(uint32_t id) {
  m_uniqueId = id;
}
// Get the unique identifier
uint32_t UniqueIdentifierHeader::GetId() const {
  return m_uniqueId;
}
// Things needed for the header
TypeId UniqueIdentifierHeader::GetTypeId(void) {
  static TypeId tid = TypeId("UniqueIdentifierHeader")
    .SetParent<Header>()
    .AddConstructor<UniqueIdentifierHeader>();
  return tid;
}
TypeId UniqueIdentifierHeader::GetInstanceTypeId(void) const {
  return GetTypeId();
}
void UniqueIdentifierHeader::Serialize(Buffer::Iterator start) const {
  start.WriteHtonU32(m_uniqueId);
}
uint32_t UniqueIdentifierHeader::Deserialize(Buffer::Iterator start) {
  m_uniqueId = start.ReadNtohU32();
  return GetSerializedSize();
}
uint32_t UniqueIdentifierHeader::GetSerializedSize(void) const {
  return sizeof(uint32_t);
}
void UniqueIdentifierHeader::Print(std::ostream &os) const {
  os << "UniqueId=" << m_uniqueId;
}



/**
 * Custom OnOffApplication class to add a UniqueIdentifierHeader to packets before sending implementation
 */

TypeId OnOffApplicationWithExtraHeader::GetTypeId(void) {
  static TypeId tid = TypeId("ns3::OnOffApplicationWithExtraHeader")
    .SetParent<OnOffApplication>()
    .AddConstructor<OnOffApplicationWithExtraHeader>();
  return tid;
}

OnOffApplicationWithExtraHeader::OnOffApplicationWithExtraHeader() {}

OnOffApplicationWithExtraHeader::~OnOffApplicationWithExtraHeader() {}

void OnOffApplicationWithExtraHeader::SendPacket() {
  // Create packet and set the UniqueIdentifierHeader
  Ptr<Packet> packet;

  // NS_LOG_UNCOND("SendPacket called, g_nextUniqueId: " << g_nextUniqueId);


  // Copy of SendPacket method from OnOffApplication with the addition of the UniqueIdentifierHeader

  if (m_unsentPacket) packet = m_unsentPacket;
  else if (m_enableSeqTsSizeHeader) {
      Address from;
      Address to;
      m_socket->GetSockName(from);
      m_socket->GetPeerName(to);
      SeqTsSizeHeader header;
      UniqueIdentifierHeader m_header;
      m_header.SetId(g_nextUniqueId++);
      header.SetSeq(m_seq++);
      header.SetSize(m_pktSize);
      NS_ABORT_IF(m_pktSize < header.GetSerializedSize());
      packet = Create<Packet>(m_pktSize - header.GetSerializedSize() - m_header.GetSerializedSize());
      packet->AddHeader(m_header);
      // Trace before adding header, for consistency with PacketSink
      m_txTraceWithSeqTsSize(packet, from, to, header);
      packet->AddHeader(header);
  }
  else {
    UniqueIdentifierHeader m_header;
    m_header.SetId(g_nextUniqueId++);
    packet = Create<Packet>(m_pktSize - m_header.GetSerializedSize());
    packet->AddHeader(m_header);
  }



  int actual = m_socket->Send(packet);
  if ((unsigned)actual == m_pktSize)
  {
      m_txTrace(packet);
      m_totBytes += m_pktSize;
      m_unsentPacket = nullptr;
      Address localAddress;
      m_socket->GetSockName(localAddress);
      if (InetSocketAddress::IsMatchingType(m_peer)) {
          m_txTraceWithAddresses(packet, localAddress, InetSocketAddress::ConvertFrom(m_peer));
      }
      else if (Inet6SocketAddress::IsMatchingType(m_peer)) {
          m_txTraceWithAddresses(packet, localAddress, Inet6SocketAddress::ConvertFrom(m_peer));
      }
  }
  else {
      m_unsentPacket = packet;
  }
  m_residualBits = 0;
  m_lastStartTime = Simulator::Now();
  ScheduleNextTx();
}
