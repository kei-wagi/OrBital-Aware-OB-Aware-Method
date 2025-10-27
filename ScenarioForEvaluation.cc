#include <iostream>

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/leo-module.h"
#include "ns3/network-module.h"
#include "ns3/aodv-module.h"
#include "ns3/udp-server.h"

#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/packet-sink.h"


using namespace ns3;


Ptr<PacketSink> sink[1];                   /* Pointer to the packet sink application */
uint64_t lastTotalRx[1];                   /* The value of the last total received bytes */
double cur[1];
int coun = 0;


void

CalculateThroughput ()

{
  Time now = Simulator::Now ();                 
  std::cout << now.GetSeconds ();
  cur[0] = (sink[0]->GetTotalRx () - lastTotalRx[0]) * (double) 8 / 1e6; 
  std::cout << "\t" << cur[0];
  if (cur[0] == 0){
    coun = coun + 1; 
  }

  lastTotalRx[0] = sink[0]->GetTotalRx ();
  std::cout << std::endl;
  Simulator::Schedule (MilliSeconds (1000), &CalculateThroughput);
}



NS_LOG_COMPONENT_DEFINE ("LeoBulkSendTracingExample");

int main (int argc, char *argv[])
{

  CommandLine cmd;
  std::string orbitFile;
  std::string traceFile;

  // LeoLatLong source (34.220691598114925, 131.28560267656653);  //25km
  LeoLatLong source (34.238232, 130.815921);                   //75km
  // LeoLatLong source (34.23183347805081, 130.19792514242465);   //125km
  // LeoLatLong source (34.2303348532639, 129.65358779741624);    //175km
  // LeoLatLong source (34.19110604769924, 126.93403208509345);   //425km
  // LeoLatLong source (34.213180420981516, 124.7570552243915);   //625km
  // LeoLatLong source (34.15782698880567, 122.58413685786202);   //825km
  // LeoLatLong source (34.24962778210507, 120.4006691830242);    //1025km



  LeoLatLong destination (34.21507, 131.555741); //Ground Station
  std::string constellation = "StarlinkUser";
  uint16_t port = 9;
  double duration = 150;
  bool pcap = false;
  uint64_t ttlThresh = 0;
  std::string routingProto = "aodv";

  cmd.AddValue("orbitFile", "CSV file with orbit parameters", orbitFile);
  cmd.AddValue("traceFile", "CSV file to store mobility trace in", traceFile);
  cmd.AddValue("precision", "ns3::LeoCircularOrbitMobilityModel::Precision");
  cmd.AddValue("duration", "Duration of the simulation in seconds", duration);
  cmd.AddValue("source", "Traffic source", source);
  cmd.AddValue("destination", "Traffic destination", destination);
  cmd.AddValue("islRate", "ns3::MockNetDevice::DataRate");
  cmd.AddValue("constellation", "LEO constellation link settings name", constellation);
  cmd.AddValue("routing", "Routing protocol", routingProto);
  cmd.AddValue("ttlThresh", "TTL threshold", ttlThresh);
  cmd.AddValue("destOnly", "ns3::aodv::RoutingProtocol::DestinationOnly");
  cmd.AddValue("routeTimeout", "ns3::aodv::RoutingProtocol::ActiveRouteTimeout");
  cmd.AddValue("pcap", "Enable packet capture", pcap);
  
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName ("ns3::TcpCubic")));  

  
  std::streambuf *coutbuf = std::cout.rdbuf();
  std::ofstream out;
  out.open (traceFile);
  if (out.is_open ())
    {
      std::cout.rdbuf(out.rdbuf());
    }

  LeoOrbitNodeHelper orbit;
  NodeContainer satellites;
  if (!orbitFile.empty())
    {
      satellites = orbit.Install (orbitFile);
    }
  else
    {
        // Gen. 1
        // satellites = orbit.Install ({ LeoOrbit (550, 53, 72, 22),  LeoOrbit (540, 53.2, 72, 22),  LeoOrbit (570, 70, 36, 20),  LeoOrbit (560, 97.6, 6, 58), LeoOrbit (560, 97.6, 4, 43)});
      
        //  Gen. 2 
        satellites = orbit.Install ({ LeoOrbit (340, 53, 48, 110),  LeoOrbit (345, 46, 48, 110),  LeoOrbit (350, 38, 48, 110),  LeoOrbit (360, 96.9, 30, 120),
        LeoOrbit (525, 53, 28, 120), LeoOrbit (530, 43, 28, 120), LeoOrbit (535, 33, 28, 120), LeoOrbit (604, 148, 12, 12), LeoOrbit (614, 115.7, 18, 18)});
    }

  LeoGndNodeHelper ground;

  NodeContainer users = ground.Install (source, destination);

  NodeContainer nodes;
  nodes.Create (1);

  PointToPointHelper pointToPoint;
  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("10000p"));
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Gbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("21.5ms"));

  NetDeviceContainer devices;
  devices = pointToPoint.Install (users.Get(1), nodes.Get(0));

  LeoChannelHelper utCh;
  utCh.SetConstellation (constellation);
  NetDeviceContainer utNet = utCh.Install (satellites, users);

  InternetStackHelper stack;
  AodvHelper aodv;
  aodv.Set ("EnableHello", BooleanValue (false));
  if (ttlThresh != 0)
    {
    aodv.Set ("TtlThreshold", UintegerValue (ttlThresh));
    aodv.Set ("NetDiameter", UintegerValue (2*ttlThresh));
    }
  stack.SetRoutingHelper (aodv);

  // Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("aodv.routes", std::ios::out);
  // aodv.PrintRoutingTableAllAt (Seconds(duration), routingStream);
  // Install internet stack on nodes
  stack.Install (satellites); 
  stack.Install (users);
  stack.Install (nodes);

  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.0.0", "255.255.0.0");
  ipv4.Assign (utNet);  
  ipv4.SetBase ("10.2.1.0", "255.255.255.0");
  ipv4.Assign (devices);



  Ipv4Address remote = nodes.Get (0)->GetObject<Ipv4> ()->GetAddress (1, 0).GetLocal ();
  BulkSendHelper sender ("ns3::TcpSocketFactory",
                         InetSocketAddress (remote, port));
  // Set the amount of data to send in bytes.  Zero is unlimited.
  sender.SetAttribute ("MaxBytes", UintegerValue (0));  
  ApplicationContainer sourceApps = sender.Install (users.Get (0));
  sourceApps.Start (Seconds (0.0));

//
// Create a PacketSinkApplication and install it on node 1
//
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink1.Install (nodes.Get (0));

  sink[0] = StaticCast<PacketSink>(sinkApps.Get(0));
  sinkApps.Start (Seconds (0.0));

  Simulator::Schedule (Seconds (1.0), &CalculateThroughput);


  if (pcap)
    {
      utCh.EnablePcap ("tcp-bulk-send", users,  false);
      pointToPoint.EnablePcap ("tcp-bulk-send", nodes,  false);
    }

  std::cout << "CONSTELLATION =" << constellation << std::endl;
  std::cout << "DURATION =" << duration << std::endl;
  std::cout << "LOCAL =" << users.Get (0)->GetId () << std::endl;
  std::cout << "REMOTE=" << nodes.Get (0)->GetId () << ",addr=" << Ipv4Address::ConvertFrom (remote) << std::endl;


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (duration));
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  std::cout <<users.Get (0)->GetId () << ":" << users.Get (1)->GetId () << ": " << sink[0]->GetTotalRx () << std::endl;

  out.close ();
  std::cout.rdbuf(coutbuf);

  return 0;
}
