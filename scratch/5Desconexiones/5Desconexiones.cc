#include "meassureFunctions.h"
#include "disconnections.h"

using namespace ns3;

// Control flags
bool detailedPrinting = false;    // Flag for detailed printing

// Simulation parameters
float stopSendingTime = 60;                               // Time to stop sending packets
float stopTime = 60;                                      // Simulation stop time
int timeInterval = 30;                                    // Time interval for printing statistics
int timeIntervalInit = 10;                                // Initial time for printing statistics
StringValue p2pDelay = StringValue("2ms");                // Delay for point-to-point links
StringValue p2pDataRate = StringValue("10Gbps");         // Data rate for point-to-point links
StringValue onoffDataRate = StringValue("100Kbps");       // Data rate for OnOff applications
UintegerValue onoffPacketSize = UintegerValue(1500);      // Packet size for OnOff applications
StringValue CCAlgorithm = StringValue("ns3::TcpNewReno"); // Congestion control algorithm
int nodesToDisconnect = 10;                               // Number of nodes to disconnect
// Error rate for package loss
Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();  // Error model for point-to-point links
DoubleValue errorRate = DoubleValue(0.0000);             // Error rate for package loss

// Directory for topology files
std::string topologyDirectory = "scratch/topologies/";

// Directory for data files
std::string dataDirectory = "data/";
std::string dataFile = "5Desconexiones.dat";
std::ofstream outputFile;

// Default values for command line parameters
std::string format("Inet");
std::string input("19nodes.txt");

// Main function
int main(int argc, char* argv[]) {
  // Randomize the seed based on current time
  uint32_t seed = static_cast<uint32_t>(time(NULL)); // Replace with your desired seed value 12345
  SeedManager::SetSeed(seed);

  // ------------------------------------------------------------
  // -- Read topology data.
  // ------------------------------------------------------------

  // Pick a topology reader based on the requested format.
  TopologyReaderHelper topoHelp;
  topoHelp.SetFileName(topologyDirectory + input);
  topoHelp.SetFileType(format);
  Ptr<TopologyReader> inFile = topoHelp.GetTopologyReader();

  // NodeContainer nodes;

  // Read the topology from the input file
  if (inFile) {
    nodes = inFile->Read();
  }

  // Check if the topology file is read successfully
  if (inFile->LinksSize() == 0) {
    std::cout << "Problems reading the topology file. Failing." << std::endl;
    return -1;
  }

  uint32_t totalNodes = nodes.GetN();

  std::cout << "Number of nodes: " << totalNodes << std::endl;

  // ------------------------------------------------------------
  // -- Create nodes and network stacks
  // ------------------------------------------------------------

  InternetStackHelper stack;

  Ipv4NixVectorHelper nixRouting;
  stack.SetRoutingHelper(nixRouting);
  stack.Install(nodes);

  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.252");

  int totlinks = inFile->LinksSize();

  auto nc = new NodeContainer[totlinks];
  TopologyReader::ConstLinksIterator iter;
  int i = 0;
  for (iter = inFile->LinksBegin(); iter != inFile->LinksEnd(); iter++, i++) {
    // Create a NodeContainer for each link
    nc[i] = NodeContainer(iter->GetFromNode(), iter->GetToNode());
  }

  // Set error rate
  em->SetAttribute("ErrorRate", errorRate);

  // Disconnect nodes
  for (int i = 0; i < nodesToDisconnect; i++) {
    DisconnectRandomNode();
  }

  // Create a set of disconnected links
  // std::set<int> disconnectedLinks;
  // for (uint32_t i=0; i<totalNodes; i++) {
  //   if (disconnectedNodes.find(i) != disconnectedNodes.end()) {
  //     Ptr<Node> node = nodes.Get(i);
  //     for (int j=0; j<totlinks; j++) {
  //       if (nc[j].Get(0) == node || nc[j].Get(1) == node) {
  //         disconnectedLinks.insert(j);
  //       }
  //     }
  //   }
  // }


  // Create point-to-point links between nodes
  auto ndc = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  for (int i = 0; i < totlinks; i++) {
    // Configure point-to-point communication attributes
    p2p.SetChannelAttribute("Delay", p2pDelay);
    p2p.SetDeviceAttribute("DataRate", p2pDataRate);
    p2p.SetDeviceAttribute("ReceiveErrorModel", PointerValue(em));

    // Set the queue to 0 for disconnected nodes links
    // if (disconnectedLinks.find(i) != disconnectedLinks.end()) {
    //   std::cout << "Setting queue size to 0 for link " << i << std::endl;
    //   p2p.SetQueue("ns3::DropTailQueue", "MaxSize", StringValue("0p"));
    // }

    // Install net devices for point-to-point communication
    ndc[i] = p2p.Install(nc[i]);
  }

  auto ipic = new Ipv4InterfaceContainer[totlinks];
  for (int i = 0; i < totlinks; i++) {
    // Assign IPv4 addresses to net devices
    ipic[i] = address.Assign(ndc[i]);
    address.NewNetwork();
  }

  // Select a random server node

  // Ptr<UniformRandomVariable> unifRandom = CreateObject<UniformRandomVariable>();
  // unifRandom->SetAttribute("Min", DoubleValue(0));
  // unifRandom->SetAttribute("Max", DoubleValue(totalNodes - 1));
  // unsigned int randomServerNumber = unifRandom->GetInteger(0, totalNodes - 1);
  // Ptr<Node> randomServerNode = nodes.Get(randomServerNumber);
  // Ptr<Ipv4> ipv4Server = randomServerNode->GetObject<Ipv4>();
  // Ipv4InterfaceAddress iaddrServer = ipv4Server->GetAddress(1, 0);
  // Ipv4Address ipv4AddrServer = iaddrServer.GetLocal();

  // ------------------------------------------------------------
  // -- Send around packets
  // ------------------------------------------------------------

  // Use Congestion Control
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", CCAlgorithm);

  // Set up TCP sockets for packet transmission
  Config::SetDefault("ns3::TcpSocket::SegmentSize", onoffPacketSize);

  // Create a container to hold all sink applications
  ApplicationContainer sinkApps;
  ApplicationContainer onOffApps;



  // Create a packet sink for each node to measure packet reception
  for (unsigned int i = 0; i < totalNodes; i++) {
    Ptr<Node> clientNode = nodes.Get(i);

    // Get the IPv4 address of the client node
    Ptr<Ipv4> ipv4Client = clientNode->GetObject<Ipv4>();
    Ipv4InterfaceAddress iaddrClient = ipv4Client->GetAddress(1, 0);
    Ipv4Address ipv4AddrClient = iaddrClient.GetLocal();

    // Set up OnOff applications for packet transmission using TCP
    OnOffHelper onoff("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrClient, 9)));
    onoff.SetAttribute("DataRate", onoffDataRate);
    onoff.SetAttribute("PacketSize", onoffPacketSize);
    onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    // Choose the next node as destination
    uint32_t destNodeIndex = (i + 1) % totalNodes;
    AddressValue remoteAddress(InetSocketAddress(nodes.Get(destNodeIndex)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal(), 9));
    onoff.SetAttribute("Remote", remoteAddress);


    // Install OnOff applications on client nodes
    ApplicationContainer onOffApp = onoff.Install(clientNode);
    onOffApps.Add(onOffApp);

    // Install sinks
    PacketSinkHelper sink("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrClient, 9)));
    ApplicationContainer sinkApp = sink.Install(clientNode);
    // sinkApp.Start(Seconds(0.0));
    // sinkApp.Stop(Seconds(stopTime));

    // Connect the custom callback function to the PacketSink's Rx trace
    Ptr<PacketSink> sinkPtr = DynamicCast<PacketSink>(sinkApp.Get(0));
    // // Create a SinkRxWithNodeId object for this node
    // SinkRxWithNodeId sinkRxWithNodeId(i);
    // Connect SinkRxWithNodeId object to the trace source
    // if (disconnectedNodes.find(i) != disconnectedNodes.end()) sinkPtr->TraceConnectWithoutContext("Rx", MakeCallback(&disconnectedSinkRx));
    // else sinkPtr->TraceConnectWithoutContext("Rx", MakeCallback(&SinkRx));

    // sinkPtr->TraceConnectWithoutContext("Rx", MakeCallback(&SinkRxWithNodeId::operator(), &sinkRxWithNodeId));
    // sinkPtr->TraceConnectWithoutContext("Rx", MakeCallback(&SinkRx));

    if (disconnectedNodes.find(i) == disconnectedNodes.end()) {
      // Iterate over net devices of the client node
      for (uint32_t j = 0; j < clientNode->GetNDevices(); ++j) {
        Ptr<NetDevice> dev = clientNode->GetDevice(j);
        Ptr<PointToPointNetDevice> p2pDev = DynamicCast<PointToPointNetDevice>(dev);
        if (p2pDev) {
            // Connect trace only for point-to-point devices
            Config::ConnectWithoutContext("/NodeList/" + std::to_string(clientNode->GetId()) +
                                          "/DeviceList/" + std::to_string(p2pDev->GetIfIndex()) +
                                          "/$ns3::PointToPointNetDevice/PhyRxEnd",
                                          MakeCallback(&nodeRxTrace));
        }
      }
    }

    // Add the sink application to the container
    sinkApps.Add(sinkApp);
  }

  // Connect trace to transmited packets
  Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyTxEnd", MakeCallback(&nodeTxTrace));
  // Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::PointToPointNetDevice/PhyRxEnd", MakeCallback(&nodeRxTrace));

  // Start the sink and OnOff applications
  sinkApps.Start(Seconds(0.0));
  sinkApps.Stop(Seconds(stopTime));

  onOffApps.Start(Seconds(0.0));
  onOffApps.Stop(Seconds(stopTime));

  // ------------------------------------------------------------
  // -- Receive packets at sink
  // ------------------------------------------------------------

  // Set up packet sink for receiving packets at the random server node
  // PacketSinkHelper sinkHelper("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrServer, 9)));
  // ApplicationContainer sinkApps = sinkHelper.Install(randomServerNode);

  // // Connect the custom callback function to the PacketSink's Rx trace
  // Ptr<PacketSink> sink = DynamicCast<PacketSink>(sinkApps.Get(0));
  // sink->TraceConnect("Rx", MakeCallback(&DropPacket));

  // Connect PacketSinkRx to the PacketSink::Rx trace source
  // Ptr<PacketSink> sink1 = sinkApps.Get(0)->GetObject<PacketSink>();
  // // sink1->AddNodeToDisconnect(clientNodes.Get(0));

  // sink1->TraceConnectWithoutContext("Rx", MakeCallback(&SinkRx));

  // sinkApps.Start(Seconds(0.0));
  // sinkApps.Stop(Seconds(stopTime));

  // ------------------------------------------------------------
  // -- Run the simulation
  // ------------------------------------------------------------

  std::cout << "Run Simulation." << std::endl;

  // Schedule reset stats
  // Simulator::Schedule(Seconds(timeIntervalInit), []() { monitor->ResetAllStats(); });

  // Open output file to print out measures
  outputFile.open(dataDirectory + dataFile, std::ios::app);
  Simulator::Schedule(Seconds(stopTime), &PrintMeasures, nodesToDisconnect,
                      std::ref(outputFile.is_open()? outputFile : std::cout));
  // for (i=timeIntervalInit+timeInterval; i<=stopTime; i+=timeInterval) {
  //   Simulator::Schedule(Seconds(i), &DisconnectRandomNode);                               // Schedule node disconnection
  //   Simulator::Schedule(Seconds(i), &PrintMeasures, detailedPrinting,
  //                       std::ref(outputFile.is_open()? outputFile : std::cout));          // Schedule measures printing
  // }


  // Stop simulation at stop time
  Simulator::Stop(Seconds(stopTime+1));
  Simulator::Run();
  Simulator::Destroy();

  // Close data file
  outputFile.close();

  // Clean up dynamically allocated arrays
  delete[] ipic;
  delete[] ndc;
  delete[] nc;

  std::cout << "Done." << std::endl;

  return 0;
}
