#include "main.h"

/*
 * Simulation Parameters
*/
float stopTime = 50;                                              // Simulation stop time
StringValue p2pDelay = StringValue("2ms");                        // Delay for point-to-point links
StringValue p2pDataRate = StringValue("100Kbps");                 // Max data rate for point-to-point links
DataRate onoffDataRate = DataRate("100Kbps");                     // Data rate for OnOff applications
uint32_t onoffPacketSize = 1500;                                  // Packet size for OnOff applications
UintegerValue TCPSegmentSize = UintegerValue(onoffPacketSize);    // Packet size for OnOff applications
StringValue CCAlgorithm = StringValue("ns3::TcpNewReno");         // Congestion control algorithm
StringValue packetQueueSize = StringValue("100p");                // Packet queue size for each link
std::string queueDiscipline = "ns3::DropTailQueue";               // Queue discipline to handle excess packets
int nodesNumberToDisconnect = 0;                                  // Number of nodes to disconnect when random

std::vector<std::vector<uint32_t>> nodesToDisconnect = {{48}, {42, 123}};                           // List of nodes to disconnect
int iteration = 1;                                                // Iteration number for knowing how many nodes to disconnect

// Error rate for package loss
Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();          // Error model for point-to-point links
DoubleValue errorRate = DoubleValue(0.0001);                      // Error rate for package loss

// Default values input paramaters
std::string input("logic_exp_2.5_v1.csv");              // Input file name
std::string dataFile = "5Desconexiones.dat";            // Where to store the data
std::string disconnectionsFile = "disconnections1.txt"; // File with disconnections of nodes

// Set random seed
uint32_t seed = 123;

std::vector<uint32_t> providerNodes = {30}; // Provider nodes

/*
* Main function
*/
int main(int argc, char* argv[]) {
  // Command line parser
  CommandLine cmd;
  cmd.AddValue("nodesNumberToDisconnect", "How many nodes to disconnect randomly", nodesNumberToDisconnect);
  cmd.AddValue("input", "Topology input", input);
  cmd.AddValue("disconnectionsFile", "File with the nodes to disconnect", disconnectionsFile);
  cmd.AddValue("output", "Where to store the data", dataFile);
  cmd.AddValue("seed", "Random seed", seed);
  cmd.AddValue("stopTime", "Simulation time", stopTime);
  cmd.AddValue("iteration", "Iteration number", iteration);
  cmd.Parse(argc, argv);

  // Set the random seed
  SeedManager::SetSeed(seed);
  srand(seed);

  // /**
  //  * Read topology data
  //  */
  // Create container for device interfaces
  std::vector<std::pair<uint32_t, uint32_t>> links;
  // Read CSV file
  std::ifstream file(topologyDirectory + input);
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

  // Determine the maximum node ID to create sufficient nodes
  uint32_t maxNodeId = 0;
  for (auto &link : links) {
    if (link.first > maxNodeId) maxNodeId = link.first;
    if (link.second > maxNodeId) maxNodeId = link.second;
  }
  totalNodes = maxNodeId + 1;
  // Print number of nodes
  // std::cout << "Total nodes: " << totalNodes << std::endl;

  // Create nodes
  nodes.Create(totalNodes);

  /*
   * Create nodes and network stacks
  */
  InternetStackHelper stack;
  Ipv4NixVectorHelper nixRouting;
  stack.SetRoutingHelper(nixRouting);
  stack.Install(nodes);

  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.252");

  // Create containers for the links
  int totlinks = links.size();
  auto nodeCont = new NodeContainer[totlinks];
  for (int i = 0; i < totlinks; ++i) {
    nodeCont[i] = NodeContainer(nodes.Get(links[i].first), nodes.Get(links[i].second));
  }

  // Set error rate
  em->SetAttribute("ErrorRate", errorRate);


  /**
   * Disconnect nodes and check connectivity to provider nodes
   */
  // Disconnect nodes
  // for (int i = 0; i < nodesNumberToDisconnect; i++) {
  //   DisconnectRandomNode();
  // }
  nodesToDisconnect = loadNodesToDisconnect(topologyDirectory + disconnectionsFile);
  for (int i=0; i<iteration; i++) {
    DisconnectNodes(nodesToDisconnect[i]);
  }

  // Check connectivity to provider nodes using BFS
  std::set<uint32_t> allReachableNodes;
  for (auto providerId : providerNodes) {
    Ptr<Node> providerNode = nodes.Get(providerId);
    std::set<uint32_t> reachableFromProvider = GetReachableNodes(providerNode, links, disconnectedNodes);

    // Union the reachable nodes from this provider to the overall set
    allReachableNodes.insert(reachableFromProvider.begin(), reachableFromProvider.end());
  }
  // Calculate the fraction of nodes that are still connected to a provider
  fractionConnectedG_L = static_cast<double>(allReachableNodes.size()) / static_cast<double>(totalNodes - disconnectedNodes.size());



  /**
   * Point-to-point links and configure communication attributes
  */
  // Create container for point-to-point links
  auto netDeviceCont = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  // Create a container to hold the addresses
  auto ipv4InterfaceCont = new Ipv4InterfaceContainer[totlinks];
  for (int i = 0; i < totlinks; ++i) {
    // Configure point-to-point communication attributes
    p2p.SetChannelAttribute("Delay", p2pDelay);
    p2p.SetDeviceAttribute("DataRate", p2pDataRate);
    p2p.SetDeviceAttribute("ReceiveErrorModel", PointerValue(em));
    p2p.SetQueue(queueDiscipline, "MaxSize", packetQueueSize);

    // Install net devices for point-to-point communication
    netDeviceCont[i] = p2p.Install(nodeCont[i]);

    // Get the queue associated with the net device
    Ptr<Queue<Packet>> queue = netDeviceCont[i].Get(0)->GetObject<PointToPointNetDevice>()->GetQueue();

    // Callback to the trace event for packets entering queue
    // queue->TraceConnectWithoutContext("Enqueue", MakeCallback(&nodeQueueEnqueueTrace));
    // queue->TraceConnectWithoutContext("Dequeue", MakeCallback(&nodeQueueDequeueTrace));

    // Callback to the trace function for queue length
    // queue->TraceConnect("PacketsInQueue", std::to_string(i), MakeCallback(&QueueLengthTrace));

    // Assign IPv4 addresses to net devices
    ipv4InterfaceCont[i] = address.Assign(netDeviceCont[i]);
    address.NewNetwork();

    // Get the endpoints of the current link
    Ptr<Node> fromNode = nodeCont[i].Get(0);

    // Check if either endpoint is in the set of disconnected nodes
    bool isDisconnected = (disconnectedNodes.find(fromNode->GetId()) != disconnectedNodes.end());

    // Set queue policy to drop all packets
    if (!isDisconnected) {
      DataRate rate = DataRate(p2pDataRate.Get());
      // Add the data rate to the total bandwidth
      totalBandwidth += rate.GetBitRate();
    }
  }

  /*
   *  Send around packets and receive them
  */
  // Use Congestion Control
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", CCAlgorithm);

  // Set up TCP sockets for packet transmission
  Config::SetDefault("ns3::TcpSocket::SegmentSize", TCPSegmentSize);

  // Create a container to hold all sink applications
  ApplicationContainer sinkApps;
  ApplicationContainer onOffApps;

  // Create a packet sink for each node to measure packet reception
  for (unsigned int i = 0; i < totalNodes; ++i) {
    Ptr<Node> clientNode = nodes.Get(i);

    // Get the IPv4 address of the client node
    Ptr<Ipv4> ipv4Client = clientNode->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddrClient = ipv4Client->GetAddress(1, 0);
    Ipv4Address ipv4AddrClient = iaddrClient.GetLocal();

    // Add packet send and connect callback only to connected nodes
    if (disconnectedNodes.find(i) == disconnectedNodes.end()) {
      // Set up OnOff applications for packet transmission using TCP
      Ptr<OnOffApplicationWithExtraHeader> onOffApp = CreateObject<OnOffApplicationWithExtraHeader>();
      // Set the packet size and data rate for the OnOff application
      onOffApp->SetAttribute("DataRate", DataRateValue(onoffDataRate));
      onOffApp->SetAttribute("PacketSize", UintegerValue(onoffPacketSize));

      // Choose the next node as destination
      uint32_t destNodeIndex = (i + 1) % totalNodes;
      onOffApp->SetAttribute("Remote", AddressValue(InetSocketAddress(nodes.Get(destNodeIndex)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), 9)));
      // Connect trace to transmitted packets at the application layer
      onOffApp->TraceConnectWithoutContext("Tx", MakeCallback(&applicationTxTrace));

      // Install custom OnOff applications on client nodes
      clientNode->AddApplication(onOffApp);
      onOffApps.Add(onOffApp);

      // Create the PacketSink application
      Ptr<PacketSink> sinkApp = CreateObject<PacketSink>();
      // Set the address of the sink application
      sinkApp->SetAttribute("Local", AddressValue(InetSocketAddress(ipv4AddrClient, 9)));
      // Connect trace to received packets at the application layer
      sinkApp->TraceConnectWithoutContext("Rx", MakeCallback(&applicationRxTrace));
      // Install the sink application on the client node
      clientNode->AddApplication(sinkApp);
      sinkApps.Add(sinkApp);

      // Connect trace to received packets
      // Iterate over net devices of the client node
      for (uint32_t j = 0; j < clientNode->GetNDevices(); ++j) {
        Ptr<NetDevice> dev = clientNode->GetDevice(j);
        Ptr<PointToPointNetDevice> p2pDev = DynamicCast<PointToPointNetDevice>(dev);
        if (p2pDev) {
          uint32_t nodeNumber = clientNode->GetId(); // Get the node number
          uint32_t deviceNumber = p2pDev->GetIfIndex(); // Get the device number

          // Connect trace for point-to-point devices to trace receiving packets at the physical layer
          Config::ConnectWithoutContext("/NodeList/" + std::to_string(nodeNumber) +
                                        "/DeviceList/" + std::to_string(deviceNumber) +
                                        "/$ns3::PointToPointNetDevice/PhyRxEnd",
                                        MakeCallback(&nodeRxTrace));
        }
      }
    }
  }

  // Connect trace to transmited packets at the physical layer
  Config::Connect("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxEnd", MakeCallback(&nodeTxTrace));

  /**
   * Start applications
  */
  // Start the sink applications
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(stopTime+1));
  // Start the onOff applications
  onOffApps.Start(Seconds(0.0));
  onOffApps.Stop(Seconds(stopTime));

  /**
   * Schedule simulation run and print function
  */
  std::cout << "Run Simulation." << std::endl;

  // Schedule progress update
  for (int i = 0; i < stopTime + 1; i++) {
    Simulator::Schedule(Seconds(i), &PrintProgress, i, stopTime);
  }


  // Schedule print measures at the end of simulation
  outputFile.open(dataDirectory + dataFile, std::ios::app);
  Simulator::Schedule(Seconds(stopTime), &PrintMeasures, disconnectedNodes,
                      std::ref(outputFile.is_open()? outputFile : std::cout), stopTime, nodesDisconnectedString);

  // Stop simulation at stop time
  Simulator::Stop(Seconds(stopTime));
  // Run the simulation
  Simulator::Run();
  Simulator::Destroy();

  // Close data file
  outputFile.close();

  // Clean up dynamically allocated arrays
  delete[] ipv4InterfaceCont;
  delete[] netDeviceCont;
  delete[] nodeCont;

  std::cout << "\nDone." << std::endl;

  return 0;
}

