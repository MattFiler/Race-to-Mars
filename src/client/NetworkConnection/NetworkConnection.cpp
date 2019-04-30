#include "NetworkConnection.h"
#include "../game.h"
#include <Engine/Renderer.h>
#include <iostream>

NetworkConnection::~NetworkConnection()
{
  client.disconnect();
  enetpp::global_state::get().deinitialize();
}

// Initialise networking and connect to server
void NetworkConnection::connectToServer(const std::string& hostname,
                                        enet_uint16 port)
{
  enetpp::global_state::get().initialize();
  client.connect(enetpp::client_connect_params()
                   .set_channel_count(1)
                   .set_server_host_name_and_port(hostname.c_str(), port));
}

// Start listening to our server
void NetworkConnection::startListening(RaceToSpace* game_instance)
{
  game = game_instance;

  // Thread out our network loop so we can continue with the game loop
  std::thread th(&NetworkConnection::networkLoop, this);
  th.detach();
}

// Our network connection loop
void NetworkConnection::networkLoop()
{
  while (client.is_connecting_or_connected())
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    using namespace std::placeholders;
    client.consume_events(std::bind(&RaceToSpace::connection, game),
                          std::bind(&RaceToSpace::disconnection, game),
                          std::bind(&RaceToSpace::data, game, _1, _2));

    while (!pkt_queue.empty())
    {
      // lock thread
      std::lock_guard<std::mutex> lock(pkt_queue_mtx);
      // take the packet first in line.
      auto& pkt = pkt_queue.front();
      // assert(sizeof(char) == sizeof(enet_uint8));
      auto pkt_length = static_cast<unsigned int>(pkt.length());
      // prepare packet to be send to server.
      auto pkt_data = pkt.data();
      // send packet to server.
      client.send_packet(0,
                         reinterpret_cast<const enet_uint8*>(pkt_data),
                         pkt_length,
                         ENET_PACKET_FLAG_RELIABLE);
      pkt_queue.pop();
    }
  }
  exiting = true;
}

/* Send our data to the server */
void NetworkConnection::sendData(DataShare& data)
{
  Packet packet;
  packet << data;
  getPacketQueue()->push(packet);
}

void NetworkConnection::sendMsg(const std::string& data)
{
  Packet packet;
  packet << data;
  getPacketQueue()->push(packet);
}
