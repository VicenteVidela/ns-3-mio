#ifndef meassureFunctions_H
#define meassureFunctions_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/error-model.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/header.h"

#include <iostream>
#include <fstream>
#include <numeric>

using namespace ns3;


// Variables for statistics

extern uint32_t receivedPacketCount;    // Number of packets received
extern uint32_t transmitedPacketCount;  // Number of packets transmitted
extern double totalBandwidth;           // Total maximum bandwidth of the network
extern uint32_t onoffPacketSize;        // Packet size for OnOff applications
extern uint32_t totalBytesReceived;     // Total bytes received

class UniqueIdentifierHeader : public Header {
public:
  UniqueIdentifierHeader();

  void SetUniqueId(uint32_t id);
  uint32_t GetUniqueId() const;

  static TypeId GetTypeId(void);
  virtual TypeId GetInstanceTypeId(void) const;

  virtual void Serialize(Buffer::Iterator start) const;
  virtual uint32_t Deserialize(Buffer::Iterator start);
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Print(std::ostream &os) const;

private:
  uint32_t m_uniqueId;
};


// Function to print throughput and packet statistics at the end of the simulation
void PrintMeasures(std::set<uint32_t> nodesDisconnected, std::ostream& output, float stopTime, std::string nodesDisconnectedString);


/**
 * Queue functions
*/
// Function for tracing packets entering queue
void nodeQueueEnqueueTrace(Ptr<const Packet> packet);

// Function for tracing packets leaving queue
void nodeQueueDequeueTrace(Ptr<const Packet> packet);

/**
 * Packet functions
*/
// Function to handle transmited packets at the physical layer
void nodeTxTrace(Ptr<const Packet> packet);

// Function to handle reception at each sink at the physical layer
void nodeRxTrace(Ptr<const Packet> packet);


// Function to handle transmited packets at the mac layer
void macTxTrace(Ptr<const Packet> packet);

// Function to handle reception at each sink at the mac layer
void macRxTrace(Ptr<const Packet> packet);



// Function for tracing queue length
void QueueLengthTrace(std::string context, uint32_t oldValue, uint32_t newValue);


#endif
