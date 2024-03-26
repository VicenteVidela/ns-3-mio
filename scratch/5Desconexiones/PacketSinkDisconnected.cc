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

// ApplicationContainer
// PacketSinkDisconnectedHelper::Install(Ptr<Node> node) const
// {
//   // Create a new instance of PacketSinkDisconnected
//   Ptr<Application> app = CreateObject<Application>();
//   Ptr<PacketSinkDisconnected> sink = CreateObject<PacketSinkDisconnected>();

//   // Install the PacketSinkDisconnected application
//   node->AddApplication(app);
//   // node->AddApplication(sink);

//   // Add nodes to disconnect
//   // for (auto it = m_nodesToDisconnect.begin(); it != m_nodesToDisconnect.end(); ++it) {
//   //     sink->AddNodeToDisconnect(*it);
//   //     // std::cout << "Node " << (*it)->GetId() << " will be disconnected from the network" << std::endl;
//   // }

//   std::cout << "Installing PacketSinkDisconnected on node " << node->GetId() << std::endl;

//   return ApplicationContainer(app);
// }

void
PacketSinkDisconnectedHelper::AddNodeToDisconnect (Ptr<Node> node)
{
  m_nodesToDisconnect.push_back (node);
}