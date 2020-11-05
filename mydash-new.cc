/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 TEI of Western Macedonia, Greece
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Dimitrios J. Vergados <djvergad@gmail.com>
 */

// Network topology
//
//       n0 ----------- n1
//            500 Kbps
//             5 ms
//
#include <string>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/dash-module.h"
#include "ns3/error-model.h"
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/rng-seed-manager.h"
//#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("DashExample");

int
main (int argc, char *argv[])
{
	uint32_t rng=1;
	uint32_t seed=1;
  bool tracing = true;
  bool sack=false;
  uint32_t maxBytes = 100;
  uint32_t users = 1;
  double target_dt = 35.0;
  double stopTime = 60.0;
  double errorRate=0;
  std::string linkRate = "1000Kbps";
  std::string delay = "5ms";
  std::string algorithm = "ns3::FdashClient";//origin: DashClient,other: FdashClient,--2020.8.13
  uint32_t bufferSpace = 10000000;
  std::string window = "10s";
  std::string tcpVersion="ns3::TcpNewReno";


  /*  LogComponentEnable("MpegPlayer", LOG_LEVEL_ALL);*/
  /* LogComponentEnable ("DashServer", LOG_LEVEL_ALL);
  LogComponentEnable ("DashClient", LOG_LEVEL_ALL);*/

  //
  // Allow the user to override any of the defaults at
  // run-time, via command-line arguments
  //
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
  cmd.AddValue ("maxBytes", "Total number of bytes for application to send", maxBytes);
  cmd.AddValue ("users", "The number of concurrent videos", users);
  cmd.AddValue ("targetDt", "The target time difference between receiving and playing a frame.",
                target_dt);
  cmd.AddValue ("stopTime", "The time when the clients will stop requesting segments", stopTime);
  cmd.AddValue ("linkRate",
                "The bitrate of the link connecting the clients to the server (e.g. 500kbps)",
                linkRate);
  cmd.AddValue ("delay", "The delay of the link connecting the clients to the server (e.g. 5ms)",
                delay);
  cmd.AddValue ("algorithms",
                "The adaptation algorithms used. It can be a comma seperated list of"
                "protocolos, such as 'ns3::FdashClient,ns3::OsmpClient'."
                "You may find the list of available algorithms in src/dash/model/algorithms",
                algorithm);
  cmd.AddValue ("bufferSpace", "The space in bytes that is used for buffering the video",
                bufferSpace);
  cmd.AddValue ("window", "The window for measuring the average throughput (Time).", window);
  cmd.AddValue("sack","Enable or disable SACK option.", sack);
  cmd.AddValue("errorRate","The rate of error.",errorRate);
  cmd.AddValue("tcpVersion","The version of TCP used.", tcpVersion);
  cmd.AddValue("rng","number of rng.",rng);
  cmd.AddValue("seed","seed of random number", seed);
  cmd.Parse (argc, argv);

  RngSeedManager::SetSeed(seed);
  RngSeedManager::SetRun(rng);

  Config::SetDefault("ns3::TcpSocketBase::Sack", BooleanValue(sack));
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue(tcpVersion));
  //if use TcpNewReno, must enable the sack, otherwise->error
  //
  // Explicitly create the nodes required by the topology (shown above).
  //
  NS_LOG_INFO ("Create nodes.");
  NodeContainer nodes;
  nodes.Create (2);

  NS_LOG_INFO ("Create channels.");

  //
  // Explicitly create the point-to-point link required by the topology (shown above).
  //
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue (linkRate));
  pointToPoint.SetChannelAttribute ("Delay", StringValue (delay));
  NetDeviceContainer devices;
  devices = pointToPoint.Install (nodes);

  Ptr<RateErrorModel> em=CreateObject<RateErrorModel>();
  em->SetAttribute("ErrorRate", DoubleValue(errorRate));
  devices.Get(1)->SetAttribute("ReceiveErrorModel", PointerValue(em));//
  devices.Get(0)->SetAttribute("ReceiveErrorModel", PointerValue(em));//2020.7.31 add

  //
  // Install the internet stack on the nodes
  //
  InternetStackHelper internet;
  internet.Install (nodes);

  //
  // We've got the "hardware" in place.  Now we need to add IP addresses.
  //
  NS_LOG_INFO ("Assign IP Addresses.");
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign (devices);

  NS_LOG_INFO ("Create Applications.");

  std::vector<std::string> algorithms;
  std::stringstream ss (algorithm);
  std::string proto;
  uint32_t protoNum = 0; // The number of algorithms
  while (std::getline (ss, proto, ',') && protoNum++ < users)
    {
      algorithms.push_back (proto);
    }

  uint16_t port = 80; // well-known echo port number
  std::vector<DashClientHelper> clients;
  std::vector<ApplicationContainer> clientApps;

  for (uint32_t user = 0; user < users; user++)
    {
      DashClientHelper client ("ns3::TcpSocketFactory", InetSocketAddress (i.GetAddress (1), port),
                               algorithms[user % protoNum]);
      //client.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
      client.SetAttribute ("VideoId", UintegerValue (user + 1)); // VideoId should be positive
      client.SetAttribute ("TargetDt", TimeValue (Seconds (target_dt)));
      client.SetAttribute ("window", TimeValue (Time (window)));
      client.SetAttribute ("bufferSpace", UintegerValue (bufferSpace));

      ApplicationContainer clientApp = client.Install (nodes.Get (0));
      clientApp.Start (Seconds (0.25));
      clientApp.Stop (Seconds (stopTime));

      clients.push_back (client);
      clientApps.push_back (clientApp);
    }

  DashServerHelper server ("ns3::TcpSocketFactory",
                           InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer serverApps = server.Install (nodes.Get (1));
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (stopTime + 5.0));

 // Ptr<FlowMonitor> flowMonitor;2020.8.14
  //FlowMonitorHelper flowHelper;2020.8.14
 // flowMonitor=flowHelper.InstallAll();2020.8.14

  //
  // Set up tracing if enabled
  //
  if (tracing)
    {
      AsciiTraceHelper ascii;
      pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("dash-send.tr"));
      pointToPoint.EnablePcapAll ("dash-send", true);

    }

  //
  // Now, do the actual simulation.
  //
  NS_LOG_INFO ("Run Simulation.");
  //*
   //Simulator::Stop(Seconds(100.0));2020.8.14
   //*/

   //AnimationInterface anim("mydash-anim.xml");
   //anim.SetConstantPosition(nodes.Get(0), 1.0, 2.0);
   //anim.SetConstantPosition(nodes.Get(1), 1.0, 5.0);
  Simulator::Run ();

  //flowMonitor->SerializeToXmlFile("mydash.xml", true, true);2020.8.14

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  uint32_t k;
  for (k = 0; k < users; k++)
    {
      Ptr<DashClient> app = DynamicCast<DashClient> (clientApps[k].Get (0));
      std::cout << algorithms[k % protoNum] << "-Node: " << k;
      app->GetStats ();
    }

  return 0;
}
