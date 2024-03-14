#ifndef PacketSinkDisconnected_H
#define PacketSinkDisconnected_H

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/random-variable-stream.h"
#include <set>

using namespace ns3;

class PacketSinkDisconnected : public PacketSink {
public:
  static TypeId GetTypeId(void);
  PacketSinkDisconnected();
  virtual ~PacketSinkDisconnected ();

  void AddNodeToDisconnect(Ptr<Node> node) {
    nodesToDisconnect.insert(node);
  }

protected:
  virtual void HandleRead (Ptr<Socket> socket);

private:
  Address m_address;
  std::set<Ptr<Node>> nodesToDisconnect;
};

class PacketSinkDisconnectedHelper : public PacketSinkHelper
{
public:
  PacketSinkDisconnectedHelper (std::string protocol, ns3::Address address);

  void SetAttribute (std::string name, const AttributeValue &value);
  ApplicationContainer Install (Ptr<Node> node) const;
  void AddNodeToDisconnect (Ptr<Node> node);

private:
  ObjectFactory m_factory;
  std::vector<Ptr<Node>> m_nodesToDisconnect;
};

#endif