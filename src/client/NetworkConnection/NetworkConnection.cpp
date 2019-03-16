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
  //  std::cout << "Welcome to Race to Space! \n Please enter your Name: ";
  //  std::getline(std::cin, username);

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

  //  std::thread th3(&RaceToSpace::input, this);
  //  th.detach();
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
      const auto& pkt = pkt_queue.front();
      assert(sizeof(char) == sizeof(enet_uint8));
      unsigned int pkt_length = 0;
      // prepare packet to be send to server.
      auto pkt_data = pkt.data(pkt_length);
      pkt_queue.pop();
      // send packet to server
      client.send_packet(0,
                         reinterpret_cast<const enet_uint8*>(pkt_data),
                         pkt_length,
                         ENET_PACKET_FLAG_RELIABLE);
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

void NetworkConnection::input()
{
  //  while (!exiting)
  //  {
  //    std::string txt;
  //    std::getline(std::cin, txt);
  //    std::time_t result = std::time(nullptr);
  //    std::string time_stamp = std::asctime(std::localtime(&result));
  //    ChatMsg msg(username, txt, result);
  //    std::lock_guard<std::mutex> lock(msg_queue_mtx);
  //    msg_queue.push(std::move(msg));
  //  }
}
