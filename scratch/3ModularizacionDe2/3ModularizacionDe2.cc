#include "meassureFunctions.h"

#include <iostream>

using namespace ns3;

// Control flags
bool print = false;               // Flag for printing information
bool animation = false;           // Flag for including animation in the simulation

// Simulation parameters
float stopTime = 60;                                 // Simulation stop time
StringValue p2pDelay = StringValue("2ms");            // Delay for point-to-point links
StringValue p2pDataRate = StringValue("5Mbps");       // Data rate for point-to-point links
DataRate onoffDataRate = DataRate("15kbps");          // Data rate for OnOff applications
UintegerValue onoffPacketSize = UintegerValue(1200);  // Packet size for OnOff applications
// Error rate for package loss
Ptr<RateErrorModel> em = CreateObject<RateErrorModel>();
DoubleValue errorRate = DoubleValue(0.00001);

// Variables for statistics
uint64_t totalReceivedBytes = 0;       // Total received bytes
uint64_t totalTransmittedPackets = 0;  // Total transmitted packets
uint64_t totalReceivedPackets = 0;     // Total received packets

// Directory for topology files
char topologyDirectory[] = "scratch/topologies/";

// Main function
int main(int argc, char* argv[])
{
  // Randomize the seed based on current time
  uint32_t seed = static_cast<uint32_t>(time(NULL)); // Replace with your desired seed value 12345
  SeedManager::SetSeed(seed);

  // Default values for command line parameters
  std::string format("Inet");
  std::string input("topology1.txt");

  // Set up command line parameters used to control the experiment.
  CommandLine cmd(__FILE__);
  cmd.AddValue("format", "Format to use for data input [Orbis|Inet|Rocketfuel].", format);
  cmd.AddValue("input", "Name of the input file.", input);
  cmd.Parse(argc, argv);

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
  if (inFile)
  {
    nodes = inFile->Read();
  }

  // Check if the topology file is read successfully
  if (inFile->LinksSize() == 0)
  {
    std::cout << "Problems reading the topology file. Failing." << std::endl;
    return -1;
  }

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
  for (iter = inFile->LinksBegin(); iter != inFile->LinksEnd(); iter++, i++)
  {
    // Create a NodeContainer for each link
    nc[i] = NodeContainer(iter->GetFromNode(), iter->GetToNode());
  }

  // Set error rate
  em->SetAttribute("ErrorRate", errorRate);

  auto ndc = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  for (int i = 0; i < totlinks; i++)
  {
    // Configure point-to-point communication attributes
    p2p.SetChannelAttribute("Delay", p2pDelay);
    p2p.SetDeviceAttribute("DataRate", p2pDataRate);
    p2p.SetDeviceAttribute("ReceiveErrorModel", PointerValue(em));
    // Install net devices for point-to-point communication
    ndc[i] = p2p.Install(nc[i]);
  }

  auto ipic = new Ipv4InterfaceContainer[totlinks];
  for (int i = 0; i < totlinks; i++)
  {
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
  // -- Setup animator and store animation
  // ------------------------------------------------------------

  if (animation)
  {
    MobilityHelper mobility;
    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue(0.0),
                                  "MinY", DoubleValue(0.0),
                                  "DeltaX", DoubleValue(5.0),
                                  "DeltaY", DoubleValue(5.0),
                                  "GridWidth", UintegerValue(4),
                                  "LayoutType", StringValue("RowFirst"));
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    AnimationInterface anim("animations/1ThroughputMeassure.xml");
  }

  // ------------------------------------------------------------
  // -- Send around packets
  // ------------------------------------------------------------

  // Set up raw sockets for packet transmission
  Config::SetDefault("ns3::Ipv4RawSocketImpl::Protocol", StringValue("2"));
  InetSocketAddress dst(ipv4AddrServer);

  // Set up OnOff applications for packet transmission
  OnOffHelper onoff = OnOffHelper("ns3::Ipv4RawSocketFactory", dst);
  onoff.SetConstantRate(onoffDataRate);
  onoff.SetAttribute("PacketSize", onoffPacketSize);

  NodeContainer clientNodes;
  for (unsigned int i = 0; i < nodes.GetN(); i++)
  {
    if (i != randomServerNumber)
    {
      // Add client nodes to the container
      Ptr<Node> clientNode = nodes.Get(i);
      clientNodes.Add(clientNode);
    }
  }

  // Install OnOff applications on client nodes
  ApplicationContainer apps = onoff.Install(clientNodes);
  apps.Start(Seconds(1.0));
  apps.Stop(Seconds(stopTime));

  // Connect OnOffTx to the OnOffApplication::Tx trace source for each OnOffApplication
  for (uint32_t i = 0; i < apps.GetN(); i++) {
    Ptr<OnOffApplication> onoffApp = DynamicCast<OnOffApplication>(apps.Get(i));
    onoffApp->TraceConnectWithoutContext("Tx", MakeCallback(&OnOffTx));
  }

  // Set up packet sink for receiving packets at the random server node
  PacketSinkHelper sink = PacketSinkHelper("ns3::Ipv4RawSocketFactory", dst);
  apps = sink.Install(randomServerNode);

  // Connect PacketSinkRx to the PacketSink::Rx trace source
  Ptr<PacketSink> sink1 = DynamicCast<PacketSink>(apps.Get(0));
  sink1->TraceConnectWithoutContext("Rx", MakeCallback(&SinkRx));

  apps.Start(Seconds(0.0));
  apps.Stop(Seconds(stopTime+1.0));

  // ------------------------------------------------------------
  // -- Run the simulation
  // ------------------------------------------------------------

  std::cout << "Run Simulation." << std::endl;
  Simulator::Schedule(Seconds(stopTime+1), &PrintMeasures); // Schedule throughput printing
  Simulator::Stop(Seconds(stopTime+1));                        // Stop simulation at 3 seconds
  Simulator::Run();
  Simulator::Destroy();

  // Clean up dynamically allocated arrays
  delete[] ipic;
  delete[] ndc;
  delete[] nc;

  std::cout << "Done." << std::endl;

  return 0;
}
