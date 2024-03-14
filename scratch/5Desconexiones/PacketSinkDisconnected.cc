#include "PacketSinkDisconnected.h"

// ------------------------------------------------------------
// -- Packet sink disconnected
// ------------------------------------------------------------
TypeId
PacketSinkDisconnected::GetTypeId (void)
{
  static TypeId tid = TypeId ("PacketSinkDisconnected")
    .SetParent<PacketSink> ()
    .AddConstructor<PacketSinkDisconnected> ()
    ;
  return tid;
}

PacketSinkDisconnected::PacketSinkDisconnected () : PacketSink() {}

PacketSinkDisconnected::~PacketSinkDisconnected () {}

void
PacketSinkDisconnected::HandleRead (Ptr<Socket> socket)
{
  if (nodesToDisconnect.find(socket->GetNode()) != nodesToDisconnect.end()) {
    // Drop packet
    return;
  }

  PacketSink::HandleRead(socket);
}




// ------------------------------------------------------------
// -- Packet sink disconnected helper
// ------------------------------------------------------------
PacketSinkDisconnectedHelper::PacketSinkDisconnectedHelper (std::string protocol, ns3::Address address)
  : PacketSinkHelper (protocol, address)
{
}


void
PacketSinkDisconnectedHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
PacketSinkDisconnectedHelper::Install(Ptr<Node> node) const
{
    ApplicationContainer apps;

    // Create a new instance of PacketSinkDisconnected
    Ptr<PacketSinkDisconnected> sink = CreateObject<PacketSinkDisconnected>();

    // Add nodes to disconnect
    for (auto it = m_nodesToDisconnect.begin(); it != m_nodesToDisconnect.end(); ++it) {
        sink->AddNodeToDisconnect(*it);
    }

    // Install the PacketSinkDisconnected application
    node->AddApplication(sink);

    // Add the installed application to the ApplicationContainer
    apps.Add(sink);

    return apps;
}

void
PacketSinkDisconnectedHelper::AddNodeToDisconnect (Ptr<Node> node)
{
  m_nodesToDisconnect.push_back (node);
}