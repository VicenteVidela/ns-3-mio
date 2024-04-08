#include "disconnections.h"

NodeContainer clientNodes;                            // Container for client nodes
std::set<uint32_t> disconnectedNodes;                 // Set to store disconnected nodes

// Function to handle reception at each sink
void SinkRx(Ptr<const Packet> packet, const Address &addr) {
  // This function will be called whenever a packet is received at any sink
  // if (disconnectedNodes.find(nodeId) != disconnectedNodes.end()){
  //   std::cout << "Packet dropped at " << nodeId << std::endl;
  // }
  std::cout << "Packet received at sink with address " << addr << std::endl;
}

// Function to disconnect a random node
void DisconnectRandomNode() {
  // Check if all nodes are already disconnected
  uint32_t totalDisconnectedNodes = disconnectedNodes.size();
  if (totalDisconnectedNodes >= clientNodes.GetN()) {
    return;
  }
  // Get a random node index
  uint32_t randomIndex = 17;

  // Check if the node is already disconnected
  while (disconnectedNodes.find(randomIndex) != disconnectedNodes.end()) {
    // Generate a new random index
    randomIndex = rand() % clientNodes.GetN();
  }

  // Add the node to the set of disconnected nodes
  disconnectedNodes.insert(randomIndex);
  std::cout << "Disconnecting node " << randomIndex << std::endl;
  // sinkDisconnected->AddNodeToDisconnect(clientNodes.Get(randomIndex));
  // Get the random node
  // Ptr<Node> randomNode = clientNodes.Get(randomIndex);

  // Disable all network interfaces on the node
  // Ptr<Ipv4> ipv4 = randomNode->GetObject<Ipv4>();
  // for (uint32_t i = 0; i < ipv4->GetNInterfaces(); ++i) {
  //   ipv4->SetDown(i);
  // }

}