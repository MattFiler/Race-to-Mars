#include "server.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include <gamelib/Packet.h>
#include <iostream>

RaceToSpaceServer::RaceToSpaceServer()
{
  debug_text.enabled = true;
  enetpp::global_state::get().initialize();
}

RaceToSpaceServer::~RaceToSpaceServer()
{
  network_server.stop_listening();
  enetpp::global_state::get().deinitialize();
}

void RaceToSpaceServer::initialise()
{
  debug_text.print("Initialising server on port " + std::to_string(port) + "!");

  auto init_client_func = [&](server_client& client, const char* ip) {
    client._uid = next_uid;
    next_uid++;
  };

  network_server.start_listening(
    enetpp::server_listen_params<server_client>()
      .set_max_client_count(20)
      .set_channel_count(1)
      .set_listen_port(static_cast<enet_uint16>(port))
      .set_initialize_client_function(init_client_func));
}

void RaceToSpaceServer::run()
{
  auto on_connect = ([&](server_client& client) {
    debug_text.print("Client " + std::to_string(client.get_id()) +
                     " has connected.");
  });

  auto on_disconnect = ([&](unsigned int client_id) {
    debug_text.print("Client " + std::to_string(client_id) +
                     " has disconnected.");
  });

  // while data is received do...
  auto on_data =
    ([&](server_client& client, const enet_uint8* data, size_t data_size) {
      NetworkedData data_to_send;
      Packet packet_data(data, data_size);
      packet_data >> data_to_send;

      debug_text.print("Sending data to all clients, of type " +
                       std::to_string(data_to_send.role) + ".");

      network_server.send_packet_to_all_if(
        0,
        data,
        data_size,
        ENET_PACKET_FLAG_RELIABLE,
        [&](const server_client& destination) {
          return destination.get_id() != client.get_id();
        });
    });

  // while server should not terminate
  static bool terminate = false;
  // cppcheck-suppress *
  while (!terminate)
  {
    network_server.consume_events(on_connect, on_disconnect, on_data);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}