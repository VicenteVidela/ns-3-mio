#ifndef uniqueIdHeader_H
#define uniqueIdHeader_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/error-model.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/header.h"

using namespace ns3;

extern uint32_t g_nextUniqueId;  // UniqueId for packets

// UniqueIdentifierHeader class
class UniqueIdentifierHeader : public Header {
public:
  UniqueIdentifierHeader();

  void SetId(uint32_t id);
  uint32_t GetId() const;

  static TypeId GetTypeId(void);
  virtual TypeId GetInstanceTypeId(void) const;

  virtual void Serialize(Buffer::Iterator start) const;
  virtual uint32_t Deserialize(Buffer::Iterator start);
  virtual uint32_t GetSerializedSize(void) const;
  virtual void Print(std::ostream &os) const;

private:
  uint32_t m_uniqueId;
};



// Custom OnOffApplication class to add a UniqueIdentifierHeader to packets before sending
class OnOffApplicationWithExtraHeader : public OnOffApplication {
public:
  static TypeId GetTypeId(void);
  OnOffApplicationWithExtraHeader();
  virtual ~OnOffApplicationWithExtraHeader();

protected:
  void SendPacket() override;

};



#endif
