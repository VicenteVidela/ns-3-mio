#ifndef sinkrxwithnodeid_H
#define sinkrxwithnodeid_H

#include "ns3/packet.h"
#include "ns3/address.h"
#include "disconnections.h"

class SinkRxWithNodeId {
public:
    SinkRxWithNodeId(uint32_t nodeId);
    void operator()(ns3::Ptr<const ns3::Packet> packet, const ns3::Address& addr);

private:
    uint32_t m_nodeId;
};

#endif
