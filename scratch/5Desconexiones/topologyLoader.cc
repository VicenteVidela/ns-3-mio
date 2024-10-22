#include "topologyLoader.h"

// Function to load the topology from a csv file with format "lX,lY"
std::vector<std::pair<uint32_t, uint32_t>> LoadTopology(const std::string& input) {
  // Create container for device interfaces
  std::vector<std::pair<uint32_t, uint32_t>> links;
  // Read CSV file
  std::ifstream file(input);
  std::string line;

  while (std::getline(file, line)) {
    // Parse the CSV line with format "lX,lY"
    std::istringstream ss(line);
    std::string nodeA, nodeB;
    std::getline(ss, nodeA, ',');
    std::getline(ss, nodeB, ',');

    // Convert node labels (e.g., "l0", "l50") to integers (0, 50)
    uint32_t nodeAId = std::stoi(nodeA.substr(1));
    uint32_t nodeBId = std::stoi(nodeB.substr(1));

    // Add to list of links
    links.push_back(std::make_pair(nodeAId, nodeBId));
  }

  return links;
}

// Function to load the providers from a txt file
std::vector<uint32_t> LoadProviders(const std::string& input) {
  // Create container for provider nodes
  std::vector<uint32_t> providers;
  // Read TXT file
  std::ifstream file(input);
  std::string line;

  while (std::getline(file, line)) {
    // Get provider ID
    uint32_t providerId = std::stoi(line);
    // Add to list of providers
    providers.push_back(providerId);
  }

  return providers;
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