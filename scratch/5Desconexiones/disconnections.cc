#include "disconnections.h"

// Function to disconnect a random node
void DisconnectRandomNode(NodeContainer& nodes) {
  // Select a random node
  uint32_t totalNodes = nodes.GetN();
  Ptr<UniformRandomVariable> unifRandom = CreateObject<UniformRandomVariable>();
  unifRandom->SetAttribute("Min", DoubleValue(0));
  unifRandom->SetAttribute("Max", DoubleValue(totalNodes - 1));
  unsigned int randomNodeNumber = unifRandom->GetInteger(0, totalNodes - 1);
  Ptr<Node> randomNode = nodes.Get(randomNodeNumber);

  // Disconnect the node
  randomNode->GetObject<Ipv4>()->SetDown(1);
}