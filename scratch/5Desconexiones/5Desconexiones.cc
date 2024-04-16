#include "main.h"

/*
 * Simulation Parameters
*/
float stopTime = 10;                                              // Simulation stop time
StringValue p2pDelay = StringValue("2ms");                        // Delay for point-to-point links
StringValue p2pDataRate = StringValue("100Kbps");                 // Max data rate for point-to-point links
DataRate onoffDataRate = DataRate("100Kbps");                     // Data rate for OnOff applications
uint32_t onoffPacketSize = 1500;                                  // Packet size for OnOff applications
UintegerValue TCPSegmentSize = UintegerValue(onoffPacketSize);    // Packet size for OnOff applications
StringValue CCAlgorithm = StringValue("ns3::TcpNewReno");         // Congestion control algorithm
StringValue packetQueueSize = StringValue("100p");                // Packet queue size for each link
std::string queueDiscipline = "ns3::DropTailQueue";               // Queue discipline to handle excess packets
int nodesToDisconnect = 0;                                        // Number of nodes to disconnect
// Error rate for package loss
Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();          // Error model for point-to-point links
DoubleValue errorRate = DoubleValue(0.000);                      // Error rate for package loss

// Directory for topology file
std::string dataFile = "5Desconexiones.dat";

// Default values input paramaters
std::string format("Inet");
std::string input("19nodes.txt");

/*
* Main function
*/
int main(int argc, char* argv[]) {
  // Command line parser
  CommandLine cmd;
  cmd.AddValue("nodesToDisconnect", "How many nodes to disconnect", nodesToDisconnect);
  cmd.AddValue("input", "Topology input", input);
  cmd.AddValue("output", "Where to store the data", dataFile);
  cmd.Parse(argc, argv);

  // Randomize the seed based on current time
  uint32_t seed = static_cast<uint32_t>(time(NULL));
  SeedManager::SetSeed(seed);

  /**
   * Read topology data
   */
  // Pick a topology reader based on the requested format.
  TopologyReaderHelper topoHelp;
  topoHelp.SetFileName(topologyDirectory + input);
  topoHelp.SetFileType(format);
  Ptr<TopologyReader> inFile = topoHelp.GetTopologyReader();

  // Read the topology from the input file
  if (inFile) {
    nodes = inFile->Read();
  }

  // Check if the topology file is read successfully
  if (inFile->LinksSize() == 0) {
    std::cout << "Problems reading the topology file. Failing." << std::endl;
    return -1;
  }

  // Get the number of nodes in the topology
  uint32_t totalNodes = nodes.GetN();

  std::cout << "Number of nodes: " << totalNodes << std::endl;

  /*
   * Create nodes and network stacks
  */
  InternetStackHelper stack;
  Ipv4NixVectorHelper nixRouting;
  stack.SetRoutingHelper(nixRouting);
  // Create an empty IPv4 static routing helper
  InternetStackHelper emptyStack;
  Ipv4StaticRoutingHelper emptyRoutingHelper;
  emptyStack.SetRoutingHelper(emptyRoutingHelper);

  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.252");

  int totlinks = inFile->LinksSize();

  auto nodeCont = new NodeContainer[totlinks];
  TopologyReader::ConstLinksIterator iter;
  int i = 0;
  for (iter = inFile->LinksBegin(); iter != inFile->LinksEnd(); ++iter, ++i) {
    // Create a NodeContainer for each link
    nodeCont[i] = NodeContainer(iter->GetFromNode(), iter->GetToNode());
  }

  // Set error rate
  em->SetAttribute("ErrorRate", errorRate);

  // Disconnect nodes
  for (int i = 0; i < nodesToDisconnect; i++) {
    DisconnectRandomNode();
  }

  // If a node is disconnected, set its routing helper to null
  for (unsigned int i = 0; i < totalNodes; ++i) {
      Ptr<Node> clientNode = nodes.Get(i);
      if (disconnectedNodes.find(i) != disconnectedNodes.end()) emptyStack.Install(clientNode); // Set routing helper to null
      else stack.Install(clientNode);   // Otherwise, install the stack normally
  }



  /**
   * Point-to-point links and configure communication attributes
  */

  // Create container for point-to-point links
  auto netDeviceCont = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  // Create a container to hold the addresses
  auto ipv4InterfaceCont = new Ipv4InterfaceContainer[totlinks];
  i=0;
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
    queue->TraceConnectWithoutContext("Enqueue", MakeCallback(&nodeQueueEnqueueTrace));
    queue->TraceConnectWithoutContext("Dequeue", MakeCallback(&nodeQueueDequeueTrace));

    // Callback to the trace function for queue length
    queue->TraceConnect("PacketsInQueue", std::to_string(i), MakeCallback(&QueueLengthTrace));

    // Assign IPv4 addresses to net devices
    ipv4InterfaceCont[i] = address.Assign(netDeviceCont[i]);
    address.NewNetwork();

    // Get the endpoints of the current link
    Ptr<Node> fromNode = nodeCont[i].Get(0);
    Ptr<Node> toNode = nodeCont[i].Get(1);

    // Check if either endpoint is in the set of disconnected nodes
    bool isDisconnected = (disconnectedNodes.find(fromNode->GetId()) != disconnectedNodes.end()) ||
                          (disconnectedNodes.find(toNode->GetId()) != disconnectedNodes.end());

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

  int a = 0;
  // Create a packet sink for each node to measure packet reception
  for (unsigned int i = 0; i < totalNodes; ++i) {
    Ptr<Node> clientNode = nodes.Get(i);

    // Get the IPv4 address of the client node
    Ptr<Ipv4> ipv4Client = clientNode->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddrClient = ipv4Client->GetAddress(1, 0);
    Ipv4Address ipv4AddrClient = iaddrClient.GetLocal();

    // Install sinks
    PacketSinkHelper sink("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrClient, 9)));
    ApplicationContainer sinkApp = sink.Install(clientNode);
    // Add the sink application to the container
    sinkApps.Add(sinkApp);

    // Add packet send and connect callback only to connected nodes
    if (disconnectedNodes.find(i) == disconnectedNodes.end()) {
      // Set up OnOff applications for packet transmission using TCP
      OnOffHelper onoff("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrClient, 9)));
      onoff.SetConstantRate(onoffDataRate, onoffPacketSize);

      // Choose the next node as destination
      uint32_t destNodeIndex = (i + 1) % totalNodes;
      AddressValue remoteAddress(InetSocketAddress(nodes.Get(destNodeIndex)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), 9));
      onoff.SetAttribute("Remote", remoteAddress);

      // Install OnOff applications on client nodes
      ApplicationContainer onOffApp = onoff.Install(clientNode);
      onOffApps.Add(onOffApp);

      // Connect trace to received packets
      // Iterate over net devices of the client node
      for (uint32_t j = 0; j < clientNode->GetNDevices(); ++j) {
        Ptr<NetDevice> dev = clientNode->GetDevice(j);
        Ptr<PointToPointNetDevice> p2pDev = DynamicCast<PointToPointNetDevice>(dev);
        if (p2pDev) {
          // Connect trace for point-to-point devices to trace receiving packets
          Config::ConnectWithoutContext("/NodeList/" + std::to_string(clientNode->GetId()) +
                                        "/DeviceList/" + std::to_string(p2pDev->GetIfIndex()) +
                                        "/$ns3::PointToPointNetDevice/PhyRxEnd",
                                        MakeCallback(&nodeRxTrace));

          a++;
          // DataRate rate = DataRate(p2pDataRate.Get());
          // // Add the data rate to the total bandwidth
          // totalBandwidth += rate.GetBitRate()/2;
        }
      }
    }
  }

  std::cout << "Connected Links " << a << std::endl;

  // Connect trace to transmited packets
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxEnd", MakeCallback(&nodeTxTrace));

  /**
   * Start applications
  */
  // Start the sink applications
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(stopTime));
  // Start the onOff applications
  onOffApps.Start(Seconds(0.0));
  onOffApps.Stop(Seconds(stopTime));

  /**
   * Schedule simulation run and print function
  */
  std::cout << "Run Simulation." << std::endl;

  // Schedule reset stats
  outputFile.open(dataDirectory + dataFile, std::ios::app);
  Simulator::Schedule(Seconds(stopTime), &PrintMeasures, nodesToDisconnect,
                      std::ref(outputFile.is_open()? outputFile : std::cout), stopTime);

  // Stop simulation at stop time
  Simulator::Stop(Seconds(stopTime+1));
  // Run the simulation
  Simulator::Run();
  Simulator::Destroy();

  // Close data file
  outputFile.close();

  // Clean up dynamically allocated arrays
  delete[] ipv4InterfaceCont;
  delete[] netDeviceCont;
  delete[] nodeCont;

  std::cout << "Done." << std::endl;

  return 0;
}
