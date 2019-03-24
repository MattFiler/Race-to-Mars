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
  debug_text.print("Initialising on port " + std::to_string(port) + "!");
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
    // Clients enter into the "global pool" - lobbies aren't joined until the
    // client enters the lobby menu to save idle menu players hogging lobby
    // space
    debug_text.print("Client " + std::to_string(client.get_id()) +
                     " has connected to server, in global pool.");
  });

  /* A client disconnected */
  auto on_disconnect = ([&](unsigned int client_id) {
    debug_text.print("Client " + std::to_string(client_id) +
                     " has disconnected.");

    // Leave the lobby and clean up, if the client is still in one
    disconnectFromLobby(static_cast<int>(client_id));
  });

  /* Data received from a client */
  auto on_data =
    ([&](server_client& client, const enet_uint8* data, size_t data_size) {
      NetworkedData data_to_send;
      Packet packet_data(data, data_size);
      packet_data >> data_to_send;

      // If client requests lobby info, we need to send that directly to the
      // client that wants it
      switch (data_to_send.role)
      {
        case data_roles::CLIENT_REQUESTS_TO_JOIN_LOBBY:
        {
          // Connect to a lobby
          connectToLobby(client);

          // Forward lobby data back to the client
          sendData(client,
                   client.get_id(),
                   data_roles::SERVER_GIVES_LOBBY_INFO,
                   lobbies.at(client.lobby_index).lobby_id,
                   lobbies.at(client.lobby_index).user_classes[0],
                   lobbies.at(client.lobby_index).user_classes[1],
                   lobbies.at(client.lobby_index).user_classes[2],
                   lobbies.at(client.lobby_index).user_classes[3],
                   lobbies.at(client.lobby_index).users_ready[0],
                   lobbies.at(client.lobby_index).users_ready[1],
                   lobbies.at(client.lobby_index).users_ready[2],
                   lobbies.at(client.lobby_index).users_ready[3],
                   client.client_index);

          break;
        }
          // We need to store lobby ready state before sending it out, so new
          // players are up to date
        case data_roles::CLIENT_CHANGED_LOBBY_READY_STATE:
        {
          bool did_send = false;
          for (Lobby& this_lobby : lobbies)
          {
            if (this_lobby.lobby_id == data_to_send.content[2])
            {
              this_lobby.users_ready[data_to_send.content[1]] =
                static_cast<bool>(data_to_send.content[0]);

              // See how many in the lobby are ready
              int ready_count = 0;
              for (int i = 0; i < max_lobby_size; i++)
              {
                if (this_lobby.users_ready[i])
                {
                  ready_count++;
                }
              }
              debug_text.print(
                std::to_string(ready_count) + " clients in lobby " +
                std::to_string(this_lobby.lobby_id) + " are ready to start.");
              if (ready_count == max_lobby_size)
              {
                // Everyone in lobby is ready, signal to all of them that we
                // should start the game.
                debug_text.print("Starting gameplay in lobby " +
                                 std::to_string(this_lobby.lobby_id) + ".");
                did_send = true;
                sendData(client,
                         static_cast<unsigned int>(-1),
                         data_roles::SERVER_STARTS_GAME,
                         0); // the client index to start (maybe randomise this
                             // 0-3?)
              }
              break;
            }
          }
          if (did_send)
          {
            break;
          }
          else
          {
            [[clang::fallthrough]];
          }
        }
          // Otherwise, it's a message that needs to be forwarded to everyone in
          // the lobby
        default:
        {
          // If a client is leaving a lobby, action that before letting the
          // lobby know
          if (data_to_send.role == data_roles::CLIENT_DISCONNECTING_FROM_LOBBY)
          {
            disconnectFromLobby(static_cast<int>(client.get_id()));
          }

          sendData(client,
                   static_cast<unsigned int>(-1),
                   data_to_send.role,
                   data_to_send.content[0],
                   data_to_send.content[1],
                   data_to_send.content[2],
                   data_to_send.content[3],
                   data_to_send.content[4],
                   data_to_send.content[5],
                   data_to_send.content[6],
                   data_to_send.content[7],
                   data_to_send.content[8],
                   data_to_send.content[9]);
        }
      }
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

/* Send data from server to a client, or lobby (user_id = -1), or all (user_id =
 * -2) */
void RaceToSpaceServer::sendData(server_client& client,
                                 unsigned int user_id,
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

  if (user_id == static_cast<unsigned int>(-1))
  {
    debug_text.print("Sending data to all clients in lobby " +
                     std::to_string(client.lobby_id) + ", of type " +
                     std::to_string(data_to_send.role) + ".");

    bool has_sent = false;
    for (Lobby& this_lobby : lobbies)
    {
      if (this_lobby.lobby_id == client.lobby_id)
      {
        for (int i = 0; i < max_lobby_size; i++)
        {
          if (this_lobby.user_ids[i] != -1)
          {
            network_server.send_packet_to(
              static_cast<enet_uint8>(this_lobby.user_ids[i]),
              0,
              reinterpret_cast<const enet_uint8*>(packet_to_send.data()),
              static_cast<unsigned int>(packet_to_send.length()),
              ENET_PACKET_FLAG_RELIABLE);
            has_sent = true;
          }
        }
      }
    }
    if (!has_sent)
    {
      debug_text.print("ERROR: Failed to send data to clients in requested "
                       "lobby.");
    }
  }
  else if (user_id == static_cast<unsigned int>(-2))
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
      user_id,
      0,
      reinterpret_cast<const enet_uint8*>(packet_to_send.data()),
      static_cast<unsigned int>(packet_to_send.length()),
      ENET_PACKET_FLAG_RELIABLE);
  }
}

// Connect to a lobby in the server
void RaceToSpaceServer::connectToLobby(server_client& client)
{
  // Try and join an existing lobby
  int lobby_client_index = -1;
  int lobby_index = 0;
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
      client.lobby_index = lobby_index;
      could_join_lobby = true;
      break;
    }
    lobby_index++;
  }

  // All lobbies are full, create a new one and join it
  if (!could_join_lobby)
  {
    Lobby& new_lobby = lobbies.emplace_back(latest_lobby_id);
    latest_lobby_id++;

    debug_text.print("Created new lobby instance. There are " +
                     std::to_string(lobbies.size()) + " active lobbies.");

    lobby_client_index = 0;

    new_lobby.user_ids[lobby_client_index] = client.get_id();
    new_lobby.user_count++;
    client.lobby_id = new_lobby.lobby_id;
    client.lobby_index = static_cast<int>(lobbies.size() - 1);
  }

  // Now client is in a lobby, assign them a free class
  bool classes_taken[4] = { false, false, false, false };
  for (int i = 0; i < max_lobby_size; i++)
  {
    int taken_class = lobbies.at(client.lobby_index).user_classes[i];
    if (taken_class != -1)
    {
      classes_taken[taken_class] = true;
    }
  }
  for (int i = 0; i < 4; i++)
  {
    if (!classes_taken[i])
    {
      lobbies.at(client.lobby_index).user_classes[lobby_client_index] =
        static_cast<player_classes>(i);
      break;
    }
  }
  client.client_index = lobby_client_index;

  debug_text.print(
    "Client " + std::to_string(client.get_id()) + " has joined lobby " +
    std::to_string(client.lobby_id) + " as role " +
    std::to_string(
      lobbies.at(client.lobby_index).user_classes[lobby_client_index]) +
    ".");
}

// Take client out of the lobby they were registered to
void RaceToSpaceServer::disconnectFromLobby(int client_id)
{
  int real_lobby_id = 0;
  for (Lobby& this_lobby : lobbies)
  {
    for (int i = 0; i < max_lobby_size; i++)
    {
      if (this_lobby.user_ids[i] == client_id)
      {
        // Disconnect from lobby
        this_lobby.user_count--;
        this_lobby.user_ids[i] = -1;
        this_lobby.users_ready[i] = false;
        this_lobby.user_classes[i] = player_classes::UNASSIGNED;

        debug_text.print("Client " + std::to_string(client_id) +
                         " has left lobby " +
                         std::to_string(this_lobby.lobby_id) + ".");

        // If lobby is now empty, remove it so we can put new players in
        // populated lobbies
        if (this_lobby.user_count == 0)
        {
          lobbies.erase(lobbies.begin() + real_lobby_id);
          debug_text.print("Destroying unpopulated lobby. There are " +
                           std::to_string(lobbies.size()) + " active lobbies.");
        }
      }
    }
    real_lobby_id++;
  }
}