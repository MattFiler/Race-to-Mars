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
  // Setup
  debug_text.print("Initialising server on port " + std::to_string(port) + "!");
  auto init_client_func = [&](server_client& client, const char* ip) {
    client._uid = next_uid;
    next_uid++;
  };

  // Create the first lobby
  lobbies.emplace_back(latest_lobby_id);
  latest_lobby_id++;

  // Start listening for network traffic
  network_server.start_listening(
    enetpp::server_listen_params<server_client>()
      .set_max_client_count(20)
      .set_channel_count(1)
      .set_listen_port(static_cast<enet_uint16>(port))
      .set_initialize_client_function(init_client_func));
}

void RaceToSpaceServer::run()
{
  /* A client connected */
  auto on_connect = ([&](server_client& client) {
    debug_text.print("Client " + std::to_string(client.get_id()) +
                     " has connected.");

    // Try and join an existing lobby
    int lobby_client_index = -1;
    bool could_join_lobby = false;
    for (Lobby& this_lobby : lobbies)
    {
      if (this_lobby.user_count < max_lobby_size)
      {
        for (int i = 0; i < max_lobby_size; i++)
        {
          if (this_lobby.user_ids[i] == -1)
          {
            lobby_client_index = i; // find us an empty slot (someone may have
                                    // left, so we're not in sequence)
          }
        }
        this_lobby.user_ids[lobby_client_index] = client.get_id();
        this_lobby.user_count++;
        client.lobby_id = this_lobby.lobby_id;
        could_join_lobby = true;
      }
    }

    // All lobbies are full, create a new one and join it
    if (!could_join_lobby)
    {
      lobbies.emplace_back(latest_lobby_id);
      latest_lobby_id++;

      lobby_client_index = 0;

      lobbies[static_cast<unsigned int>(latest_lobby_id) - 1]
        .user_ids[lobbies[static_cast<unsigned int>(latest_lobby_id) - 1]
                    .user_count] = client.get_id();
      lobbies[static_cast<unsigned int>(latest_lobby_id) - 1].user_count++;
      client.lobby_id =
        lobbies[static_cast<unsigned int>(latest_lobby_id) - 1].lobby_id;
    }

    // Now client is in a lobby, assign them a free class
    bool classes_taken[4] = { false, false, false, false };
    for (int i = 0; i < max_lobby_size; i++)
    {
      int taken_class = lobbies.at(client.lobby_id).user_classes[i];
      if (taken_class != -1)
      {
        classes_taken[taken_class] = true;
      }
    }
    for (int i = 0; i < 4; i++)
    {
      if (!classes_taken[i])
      {
        lobbies.at(client.lobby_id).user_classes[lobby_client_index] =
          static_cast<player_classes>(i);
        break;
      }
    }

    debug_text.print(
      "Client " + std::to_string(client.get_id()) + " has joined lobby " +
      std::to_string(client.lobby_id) + " as role " +
      std::to_string(
        lobbies.at(client.lobby_id).user_classes[lobby_client_index]) +
      ".");
  });

  /* A client disconnected */
  auto on_disconnect = ([&](unsigned int client_id) {
    debug_text.print("Client " + std::to_string(client_id) +
                     " has disconnected.");

    // Take client out of the lobby they were registered to
    for (Lobby& this_lobby : lobbies)
    {
      for (int i = 0; i < max_lobby_size; i++)
      {
        if (this_lobby.user_ids[i] == client_id)
        {
          this_lobby.user_count--;
          this_lobby.user_ids[i] = -1;
          this_lobby.users_ready[i] = false;
          this_lobby.user_classes[i] = player_classes::UNASSIGNED;

          debug_text.print("Client " + std::to_string(client_id) +
                           " has left lobby " +
                           std::to_string(this_lobby.lobby_id) + ".");
        }
      }
    }
  });

  /* Data received from a client */
  auto on_data =
    ([&](server_client& client, const enet_uint8* data, size_t data_size) {
      NetworkedData data_to_send;
      Packet packet_data(data, data_size);
      packet_data >> data_to_send;

      debug_text.print("Forwarding data to all clients, of type " +
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

  /* Our server update loop */
  static bool terminate = false;
  // cppcheck-suppress *
  while (!terminate)
  {
    network_server.consume_events(on_connect, on_disconnect, on_data);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

/* Send data from server to a client, or all (user_id = -1) */
void RaceToSpaceServer::sendData(int user_id,
                                 data_roles _role,
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
  Packet packet_to_send;
  NetworkedData data_to_send;
  data_to_send.role = _role;
  data_to_send.content[0] = _content_1;
  data_to_send.content[1] = _content_2;
  data_to_send.content[2] = _content_3;
  data_to_send.content[3] = _content_4;
  data_to_send.content[4] = _content_5;
  data_to_send.content[5] = _content_6;
  data_to_send.content[6] = _content_7;
  data_to_send.content[7] = _content_8;
  data_to_send.content[8] = _content_9;
  data_to_send.content[9] = _content_10;
  packet_to_send << data_to_send;

  if (user_id == -1)
  {
    debug_text.print("Sending data to all clients, of type " +
                     std::to_string(data_to_send.role) + ".");

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
                     ", of type " + std::to_string(data_to_send.role) + ".");

    network_server.send_packet_to(
      static_cast<unsigned int>(user_id),
      0,
      reinterpret_cast<const enet_uint8*>(packet_to_send.data()),
      static_cast<unsigned int>(packet_to_send.length()),
      ENET_PACKET_FLAG_RELIABLE);
  }
}