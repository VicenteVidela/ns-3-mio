#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/nix-vector-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/topology-read-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"

#include <iostream>

using namespace ns3;

// For printing or not
bool print = false;
// For storing animation
bool animation = false;

// Variable to store total received bytes
uint64_t totalReceivedBytes = 0;

// Topologies directory
char topologyDirectory[] = "scratch/topologies/";

// Callback function to process received packets and update totalReceivedBytes
static void SinkRx(Ptr<const Packet> p, const Address& ad)
{
  Ipv4Header ipv4;
  p->PeekHeader(ipv4);
  // Update total received bytes
  totalReceivedBytes += p->GetSize();
}

// Function to print throughput at the end of the simulation
void PrintThroughput()
{
  std::cout << "Total received bytes: " << totalReceivedBytes << std::endl;
  double throughput = static_cast<double>(totalReceivedBytes) / (Simulator::Now().GetSeconds());
  std::cout << "Throughput: " << throughput << " bytes/second" << std::endl;
}

// Main function
int main(int argc, char* argv[])
{
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
  // --------------------------------------------

  // Pick a topology reader based on the requested format.
  TopologyReaderHelper topoHelp;
  topoHelp.SetFileName(topologyDirectory+input);
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
  // --------------------------------------------

  if (print) std::cout << "Creating internet stack" << std::endl;
  InternetStackHelper stack;

  // Setup NixVector Routing
  Ipv4NixVectorHelper nixRouting;
  stack.SetRoutingHelper(nixRouting); // has effect on the next Install()
  stack.Install(nodes);

  if (print) std::cout << "Creating IPv4 addresses" << std::endl;
  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.252");

  int totlinks = inFile->LinksSize();

  if (print) std::cout << "Creating node containers" << std::endl;
  auto nc = new NodeContainer[totlinks];
  TopologyReader::ConstLinksIterator iter;
  int i = 0;
  for (iter = inFile->LinksBegin(); iter != inFile->LinksEnd(); iter++, i++)
  {
    // Create a NodeContainer for each link
    nc[i] = NodeContainer(iter->GetFromNode(), iter->GetToNode());
  }

  if (print) std::cout << "Creating net device containers" << std::endl;
  auto ndc = new NetDeviceContainer[totlinks];
  PointToPointHelper p2p;
  for (int i = 0; i < totlinks; i++)
  {
    // Configure point-to-point communication attributes
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    // Install net devices for point-to-point communication
    ndc[i] = p2p.Install(nc[i]);
  }

  // Create little subnets, one for each couple of nodes.
  if (print) std::cout << "Creating IPv4 interfaces" << std::endl;
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
  // --------------------------------------------

  if (animation) {
    MobilityHelper mobility;
    mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                  "MinX", DoubleValue (0.0),
                                  "MinY", DoubleValue (0.0),
                                  "DeltaX", DoubleValue (5.0),
                                  "DeltaY", DoubleValue (5.0),
                                  "GridWidth", UintegerValue (4),
                                  "LayoutType", StringValue ("RowFirst"));
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (nodes);

    AnimationInterface anim("animations/1ThroughputMeassure.xml");
  }

  // ------------------------------------------------------------
  // -- Send around packets to check the TTL
  // --------------------------------------------

  // Set up raw sockets for packet transmission
  Config::SetDefault("ns3::Ipv4RawSocketImpl::Protocol", StringValue("2"));
  InetSocketAddress dst(ipv4AddrServer);

  // Set up OnOff applications for packet transmission
  OnOffHelper onoff = OnOffHelper("ns3::Ipv4RawSocketFactory", dst);
  onoff.SetConstantRate(DataRate(15000));
  onoff.SetAttribute("PacketSize", UintegerValue(1200));

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
  apps.Stop(Seconds(2.0));

  // Set up packet sink for receiving packets at the random server node
  PacketSinkHelper sink = PacketSinkHelper("ns3::Ipv4RawSocketFactory", dst);
  apps = sink.Install(randomServerNode);
  apps.Start(Seconds(0.0));
  apps.Stop(Seconds(3.0));

  // Connect the packet sink receiver to extract the TTL
  Config::ConnectWithoutContext("/NodeList/*/ApplicationList/*/$ns3::PacketSink/Rx",
                                MakeCallback(&SinkRx));

  // ------------------------------------------------------------
  // -- Run the simulation
  // --------------------------------------------

  std::cout << "Run Simulation." << std::endl;
  Simulator::Schedule(Seconds(3.0), &PrintThroughput); // Schedule throughput printing
  Simulator::Stop(Seconds(3.0));                        // Stop simulation at 3 seconds
  Simulator::Run();
  Simulator::Destroy();

  // Clean up dynamically allocated arrays
  delete[] ipic;
  delete[] ndc;
  delete[] nc;

  std::cout << "Done." << std::endl;

  return 0;
}