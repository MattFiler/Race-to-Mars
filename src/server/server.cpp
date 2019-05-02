#include "server.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include <algorithm>
#include <gamelib/Math/SimpleMath.h>
#include <gamelib/Packet.h>
#include <iostream>
#include <random>

/* Initialise the server */
RaceToSpaceServer::RaceToSpaceServer()
{
  debug_text.enabled = true;
  enetpp::global_state::get().initialize();
}

/* Destroy the server */
RaceToSpaceServer::~RaceToSpaceServer()
{
  network_server.stop_listening();
  enetpp::global_state::get().deinitialize();
}

/* Start the server logic */
void RaceToSpaceServer::initialise()
{
  // Setup
  debug_text.print("Initialising on port " + std::to_string(port) + "!");
  auto init_client_func = [&](server_client& client, const char* ip) {
    client._uid = next_uid;
    next_uid++;
  };

  // Create the first lobby
  lobbies.emplace_back(latest_lobby_id);
  latest_lobby_id++;

  // create deck for first lobby here.
  initLobbyDecks();

  // Start listening for network traffic
  network_server.start_listening(
    enetpp::server_listen_params<server_client>()
      .set_max_client_count(20)
      .set_channel_count(1)
      .set_listen_port(static_cast<enet_uint16>(port))
      .set_initialize_client_function(init_client_func));
}

/* The server core functionality */
void RaceToSpaceServer::run()
{
  /* A client connected */
  auto on_connect = [&](server_client& client) {
    // Clients enter into the "global pool" - lobbies aren't joined until the
    // client enters the lobby menu to save idle menu players hogging lobby
    // space
    debug_text.print("Client " + std::to_string(client.get_id()) +
                     " has connected to server, in global pool.");
  };

  /* A client disconnected */
  auto on_disconnect = [&](unsigned int client_id) {
    debug_text.print("Client " + std::to_string(client_id) +
                     " has disconnected.");

    // Leave the lobby and clean up, if the client is still in one
    disconnectFromLobby(static_cast<int>(client_id), true);
  };

  /* Data received from a client */
  auto on_data =
    [&](server_client& client, const enet_uint8* data, size_t data_size) {
      DataShare data_to_send;
      Packet packet_data(data, data_size);
      packet_data >> data_to_send;

      handleReceivedData(data_to_send, client);
    };

  /* Our server update loop */
  static bool terminate = false;
  // cppcheck-suppress *
  while (!terminate)
  {
    network_server.consume_events(on_connect, on_disconnect, on_data);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

/* Forward data to all */
void RaceToSpaceServer::sendToAll(server_client& client, DataShare& data)
{
  sendData(client, static_cast<unsigned int>(-1), data);
}

/* Send data from server to a client, or lobby (user_id = -1), or all (user_id =
 * -2) */
void RaceToSpaceServer::sendData(server_client& client,
                                 unsigned int user_id,
                                 DataShare& data)
{
  Packet packet_to_send;
  packet_to_send << data;

  if (user_id == static_cast<unsigned int>(-1))
  {
    debug_text.print("Sending data to all clients in lobby " +
                     std::to_string(client.lobby_id) + ", of type " +
                     std::to_string(data.getType()) + ".");

    bool has_sent = false;
    int player_count = -1;
    for (Lobby& this_lobby : lobbies)
    {
      if (this_lobby.lobby_id == client.lobby_id)
      {
        player_count = this_lobby.user_count;
        for (int i = 0; i < max_lobby_size; i++)
        {
          if (this_lobby.players[i].id != -1)
          {
            network_server.send_packet_to(
              static_cast<enet_uint8>(this_lobby.players[i].id),
              0,
              reinterpret_cast<const enet_uint8*>(packet_to_send.data()),
              static_cast<unsigned int>(packet_to_send.length()),
              ENET_PACKET_FLAG_RELIABLE);
            has_sent = true;
          }
        }
      }
    }
    if (!has_sent && player_count == 0)
    {
      debug_text.print("ERROR: Failed to send data to clients in requested "
                       "lobby.",
                       2);
    }
  }
  else if (user_id == static_cast<unsigned int>(-2))
  {
    debug_text.print("Sending data to all clients, of type " +
                     std::to_string(data.getType()) + ".");

    network_server.send_packet_to_all_if(
      0,
      reinterpret_cast<const enet_uint8*>(packet_to_send.data()),
      static_cast<unsigned int>(packet_to_send.length()),
      ENET_PACKET_FLAG_RELIABLE,
      [&](const server_client& destination) { return true; });
  }
  else
  {
    debug_text.print("Sending data to client " + std::to_string(user_id) +
                     ", of type " + std::to_string(data.getType()) + ".");

    network_server.send_packet_to(
      user_id,
      0,
      reinterpret_cast<const enet_uint8*>(packet_to_send.data()),
      static_cast<unsigned int>(packet_to_send.length()),
      ENET_PACKET_FLAG_RELIABLE);
  }
}
