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

  // Enable debug input to test comms from client to server (& other clients)
  std::thread th2(&NetworkConnection::networkMessageDebug, this);
  th2.detach();
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
      assert(sizeof(char) == sizeof(enet_uint8));
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

// Allow us to send a debug message over the network to our server and any other
// clients
void NetworkConnection::networkMessageDebug()
{
  while (!exiting)
  {
    std::string txt;
    std::getline(std::cin, txt);

    std::lock_guard<std::mutex> lock(msg_queue_debug_mtx);
    msg_queue_debug.push(std::move(txt));
  }
}

/* Send a NetworkData struct of data */
void NetworkConnection::sendData(data_roles _role,
                                 int _content_1,
                                 int _content_2,
                                 int _content_3,
                                 int _content_4,
                                 int _content_5,
                                 int _content_6,
                                 int _content_7,
                                 int _content_8,
                                 int _content_9,
                                 int _content_10)
{
  NetworkedData data;
  data.role = _role;
  data.content[0] = _content_1;
  data.content[1] = _content_2;
  data.content[2] = _content_3;
  data.content[3] = _content_4;
  data.content[4] = _content_5;
  data.content[5] = _content_6;
  data.content[6] = _content_7;
  data.content[7] = _content_8;
  data.content[8] = _content_9;
  data.content[9] = _content_10;

  Packet packet;
  packet << data;
  getPacketQueue()->push(packet);
}