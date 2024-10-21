#include "disconnections.h"

NodeContainer nodes;                                // Container for client nodes
std::set<uint32_t> disconnectedNodes;     // Set to store disconnected nodes
std::string nodesDisconnectedString;           // String to store disconnected nodes in order

// Function to disconnect a random node
void DisconnectRandomNode() {
  // Check if all nodes are already disconnected
  uint32_t totalDisconnectedNodes = disconnectedNodes.size();
  if (totalDisconnectedNodes >= nodes.GetN()) {
    return;
  }
  // Get a random node index
  uint32_t randomIndex = rand() % nodes.GetN();

  // Check if the node is already disconnected
  while (disconnectedNodes.find(randomIndex) != disconnectedNodes.end()) {
    // Generate a new random index
    randomIndex = rand() % nodes.GetN();
  }

  // Add the node to the set of disconnected nodes
  disconnectedNodes.insert(randomIndex);
  nodesDisconnectedString += std::to_string(randomIndex) + ' ';
  // std::cout << "Disconnecting node " << randomIndex << std::endl;
}

// Function to disconnect a list of nodes
void DisconnectNodes(std::vector<uint32_t> nodesToDisconnect) {
  for (uint32_t nodeIndex : nodesToDisconnect) {
    // Add the node to the set of disconnected nodes
    disconnectedNodes.insert(nodeIndex);
    nodesDisconnectedString += std::to_string(nodeIndex) + ' ';
  }
}

// Function for getting all reachable nodes from startNode
std::set<uint32_t> GetReachableNodes(Ptr<Node> startNode, const std::vector<std::pair<uint32_t, uint32_t>>& links, std::set<uint32_t> disconnectedNodes) {
  std::set<uint32_t> reachableNodes;
  std::set<uint32_t> visitedNodes;
  std::queue<uint32_t> nodeQueue;
  uint32_t startNodeId = startNode->GetId();

  // Initialize the search from the starting node (if not disconnected)
  if (disconnectedNodes.find(startNodeId) == disconnectedNodes.end()) {
    nodeQueue.push(startNodeId);
    visitedNodes.insert(startNodeId);
  }

  // BFS traversal
  while (!nodeQueue.empty()) {
    uint32_t currentNodeId = nodeQueue.front();
    nodeQueue.pop();
    reachableNodes.insert(currentNodeId);

    // Iterate through all links to find neighbors
    for (const auto& link : links) {
      uint32_t neighbor = 0;

      // Determine the neighbor of the current node in the current link
      if (link.first == currentNodeId) neighbor = link.second;
      else if (link.second == currentNodeId) neighbor = link.first;
      else continue;

      // Skip this neighbor if it is disconnected or already visited
      if (disconnectedNodes.find(neighbor) != disconnectedNodes.end() || visitedNodes.find(neighbor) != visitedNodes.end())
        continue;

      // Mark this neighbor as visited and add to the queue
      visitedNodes.insert(neighbor);
      nodeQueue.push(neighbor);
    }
  }

  return reachableNodes;
}



// Function to load the nodes from the file
std::vector<std::vector<uint32_t>> loadNodesToDisconnect(const std::string& filename) {
  std::vector<std::vector<uint32_t>> nodesToDisconnect;
  std::ifstream file(filename);

  if (!file) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    return nodesToDisconnect;
  }

  std::string line;
  std::regex numberRegex(R"(l(\d+))");  // Regex to extract numbers from 'l<num>'
  std::smatch match;

  // Read the file line-by-line
  while (std::getline(file, line, ',')) {
    std::vector<uint32_t> group;
    std::string token;

    // Use regex to find all 'l<num>' patterns and extract the numbers
    std::sregex_iterator iter(line.begin(), line.end(), numberRegex);
    std::sregex_iterator end;
    while (iter != end) {
      group.push_back(std::stoi((*iter)[1]));
      ++iter;
    }

    if (!group.empty()) {
      nodesToDisconnect.push_back(group);
    }
  }

  return nodesToDisconnect;
}