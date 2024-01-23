#include "meassureFunctions.h"
#include "disconnections.h"

using namespace ns3;

// Control flags
bool detailedPrinting = false;    // Flag for detailed printing

// Simulation parameters
float stopSendingTime = 602;                             // Time to stop sending packets
float stopTime = 602;                                    // Simulation stop time
int timeInterval = 60;                                    // Time interval for printing statistics
int timeIntervalInit = 2;                                // Initial time for printing statistics
StringValue p2pDelay = StringValue("100ns");              // Delay for point-to-point links
StringValue p2pDataRate = StringValue("100Kbps");           // Data rate for point-to-point links
StringValue onoffDataRate = StringValue("10Kbps");       // Data rate for OnOff applications
UintegerValue onoffPacketSize = UintegerValue(1200);      // Packet size for OnOff applications
StringValue CCAlgorithm = StringValue("ns3::TcpNewReno"); // Congestion control algorithm
// Error rate for package loss
Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();  // Error model for point-to-point links
DoubleValue errorRate = DoubleValue(0.00005);            // Error rate for package loss

// Directory for topology files
std::string topologyDirectory = "scratch/topologies/";

// Directory for data files
std::string dataDirectory = "data/";
std::string dataFile = "5Desconexiones0.dat";
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

  NodeContainer nodes;

  // Read the topology from the input file
  if (inFile) {
    nodes = inFile->Read();
  }

  // Check if the topology file is read successfully
  if (inFile->LinksSize() == 0) {
    std::cout << "Problems reading the topology file. Failing." << std::endl;
    return -1;
  }

  std::cout << "Number of nodes: " << nodes.GetN() << std::endl;

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

  auto ndc = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  for (int i = 0; i < totlinks; i++) {
    // Configure point-to-point communication attributes
    p2p.SetChannelAttribute("Delay", p2pDelay);
    p2p.SetDeviceAttribute("DataRate", p2pDataRate);
    p2p.SetDeviceAttribute("ReceiveErrorModel", PointerValue(em));
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
  uint32_t totalNodes = nodes.GetN();
  Ptr<UniformRandomVariable> unifRandom = CreateObject<UniformRandomVariable>();
  unifRandom->SetAttribute("Min", DoubleValue(0));
  unifRandom->SetAttribute("Max", DoubleValue(totalNodes - 1));
  unsigned int randomServerNumber = unifRandom->GetInteger(0, totalNodes - 1);
  Ptr<Node> randomServerNode = nodes.Get(randomServerNumber);
  Ptr<Ipv4> ipv4Server = randomServerNode->GetObject<Ipv4>();
  Ipv4InterfaceAddress iaddrServer = ipv4Server->GetAddress(1, 0);
  Ipv4Address ipv4AddrServer = iaddrServer.GetLocal();

  // ------------------------------------------------------------
  // -- Send around packets
  // ------------------------------------------------------------

  // Use Congestion Control
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", CCAlgorithm);

  // Set up TCP sockets for packet transmission
  Config::SetDefault("ns3::TcpSocket::SegmentSize", onoffPacketSize);

  // Set up OnOff applications for packet transmission using TCP
  OnOffHelper onoff("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrServer, 9)));
  onoff.SetAttribute("DataRate", onoffDataRate);
  onoff.SetAttribute("PacketSize", onoffPacketSize);

  // Set the 'On' time to a random variable with an exponential distribution
  Ptr<ExponentialRandomVariable> onTime = CreateObject<ExponentialRandomVariable>();
  onTime->SetAttribute("Mean", DoubleValue(1.0)); // Mean 'On' time is 1 second
  onoff.SetAttribute("OnTime", PointerValue(onTime));

  // Set the 'Off' time to a random variable with an exponential distribution
  Ptr<ExponentialRandomVariable> offTime = CreateObject<ExponentialRandomVariable>();
  offTime->SetAttribute("Mean", DoubleValue(0.5)); // Mean 'Off' time is 2 seconds
  onoff.SetAttribute("OffTime", PointerValue(offTime));

  // Add client nodes to the container
  for (unsigned int i = 0; i < nodes.GetN(); i++) {
    if (i != randomServerNumber) {
      // Add client nodes to the container
      Ptr<Node> clientNode = nodes.Get(i);
      clientNodes.Add(clientNode);
    }
  }

  // Install OnOff applications on client nodes
  ApplicationContainer apps = onoff.Install(clientNodes);
  apps.Start(Seconds(1.0));
  apps.Stop(Seconds(stopSendingTime));

  // Install flow monitor on all nodes
  monitor = flowmon.InstallAll();

  // ------------------------------------------------------------
  // -- Receive packets at sink
  // ------------------------------------------------------------

  // Set up packet sink for receiving packets at the random server node
  PacketSinkHelper sink("ns3::TcpSocketFactory", Address(InetSocketAddress(ipv4AddrServer, 9)));
  apps = sink.Install(randomServerNode);

  // Connect PacketSinkRx to the PacketSink::Rx trace source
  Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(apps.Get(0));
  // sink1->TraceConnectWithoutContext("Rx", MakeCallback(&SinkRx));

  apps.Start(Seconds(0.0));
  apps.Stop(Seconds(stopTime));

  // ------------------------------------------------------------
  // -- Run the simulation
  // ------------------------------------------------------------

  std::cout << "Run Simulation." << std::endl;

  outputFile.open(dataDirectory + dataFile);                                              // Open data file
  for (i=timeIntervalInit; i<=stopTime; i+=timeInterval) {
    Simulator::Schedule(Seconds(i), &DisconnectRandomNode);                               // Schedule node disconnection
    Simulator::Schedule(Seconds(i), &PrintMeasures, detailedPrinting,
                        std::ref(outputFile.is_open()? outputFile : std::cout));          // Schedule measures printing
  }

  // Stop simulation at 3 seconds
  Simulator::Stop(Seconds(stopTime));
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
