#include "server.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include <algorithm>
#include <gamelib/Packet.h>
#include <iostream>
#include <random>

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
      DataShare data_to_send;
      Packet packet_data(data, data_size);
      packet_data >> data_to_send;

      // If client requests lobby info, we need to send that directly to the
      // client that wants it.
      switch (data_to_send.getType())
      {
        case data_roles::CLIENT_REQUESTS_TO_JOIN_LOBBY:
        {
          // Connect to a lobby
          connectToLobby(client);

          // Create data array to send
          DataShare new_share = DataShare(data_roles::SERVER_GIVES_LOBBY_INFO);
          new_share.add(lobbies.at(client.lobby_index).lobby_id);
          new_share.add(lobbies.at(client.lobby_index).players[0].class_type);
          new_share.add(lobbies.at(client.lobby_index).players[1].class_type);
          new_share.add(lobbies.at(client.lobby_index).players[2].class_type);
          new_share.add(lobbies.at(client.lobby_index).players[3].class_type);
          new_share.add(lobbies.at(client.lobby_index).players[0].is_ready);
          new_share.add(lobbies.at(client.lobby_index).players[1].is_ready);
          new_share.add(lobbies.at(client.lobby_index).players[2].is_ready);
          new_share.add(lobbies.at(client.lobby_index).players[3].is_ready);
          new_share.add(client.client_index);

          // Forward lobby data back to the client
          sendData(client, client.get_id(), new_share);
          break;
        }

          // The currently active client wants to end their turn, so we need to
          // progress the game - this may be as simple as going to the next
          // player, but if we've done a full rotation in this lobby, new issue
          // cards will need to be pulled, etc.
        case data_roles::CLIENT_WANTS_TO_END_TURN:
        {
          endTurn(client);
          break;
        }

          // Sync lobby info to a player that joined the game in progress
        case data_roles::CLIENT_REQUESTS_SYNC:
        {
          syncClient(client);
          break;
        }

          // Client spent or gained action points, update the lobby before
          // sending to all - we can use this data for reconnecting players to
          // keep them updated on game progress.
        case data_roles::CLIENT_ACTION_POINTS_CHANGED:
        {
          Lobby* this_clients_lobby = getLobbyByID(client.lobby_id);
          if (this_clients_lobby == nullptr)
          {
            break;
          }

          // Update client's action point count for us
          this_clients_lobby->players[data_to_send.retrieve(0)].action_points =
            data_to_send.retrieve(1);
          debug_text.print(
            "Client " + std::to_string(data_to_send.retrieve(0)) + " now has " +
            std::to_string(data_to_send.retrieve(1)) + " action points.");

          sendToAll(client, data_to_send);
          break;
        }

          // If a client is leaving a lobby, action that before letting the
          // lobby know
        case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
        {
          disconnectFromLobby(static_cast<int>(client.get_id()));
          sendToAll(client, data_to_send);
          break;
        }

        // If client moves in the ship, save that first for syncing to
        // reconnecting players
        case data_roles::CLIENT_MOVING_PLAYER_TOKEN:
        {
          Lobby* this_lobby = getLobbyByID(client.lobby_id);
          if (this_lobby == nullptr)
          {
            break;
          }
          debug_text.print(
            "Client " + std::to_string(data_to_send.retrieve(0)) +
            " is now in room " + std::to_string(data_to_send.retrieve(1)));
          this_lobby->players[data_to_send.retrieve(0)].room_position =
            static_cast<ship_rooms>(data_to_send.retrieve(1));

          sendToAll(client, data_to_send);
          break;
        }
          // Updates the new progress index for all clients after dice roll
          // issue event on client.
        case data_roles::CLIENT_CHANGE_PROGRESS_INDEX:
        {
          Lobby* this_lobby = getLobbyByID(client.lobby_id);
          if (this_lobby == nullptr)
          {
            break;
          }
          debug_text.print("Changing progress from: " +
                           std::to_string(this_lobby->current_progress_index) +
                           " to:" + std::to_string(data_to_send.retrieve(0)));
          this_lobby->current_progress_index = data_to_send.retrieve(0);

          // Compile and send it on
          DataShare new_share =
            DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
          new_share.add(data_to_send.retrieve(0));
          sendData(client, static_cast<unsigned int>(-2), new_share);
          break;
        }

        // Client has requested to buy an item.
        case data_roles::CLIENT_REQUESTED_ITEM_CARD:
        {
          Lobby* this_lobby = getLobbyByID(client.lobby_id);
          if (this_lobby == nullptr)
          {
            break;
          }

          // Compile data and send it back off
          DataShare new_share =
            DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
          new_share.add(data_to_send.retrieve(0));
          new_share.add(this_lobby->item_deck.back());

          sendData(client, static_cast<unsigned int>(-2), new_share);
          break;
        }
          // We need to store lobby ready state before sending it out, so new
          // players are up to date
        case data_roles::CLIENT_CHANGED_LOBBY_READY_STATE:
        {
          clientReadyUp(data_to_send, client);
          break;
        }
          // Otherwise, it's a message that needs to be forwarded to everyone in
          // the lobby.
        default:
        {
          sendToAll(client, data_to_send);
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

/* Forward data to all */
void RaceToSpaceServer::sendToAll(server_client& client, DataShare data)
{
  sendData(client, static_cast<unsigned int>(-1), data);
}

/* Send data from server to a client, or lobby (user_id = -1), or all (user_id =
 * -2) */
void RaceToSpaceServer::sendData(server_client& client,
                                 unsigned int user_id,
                                 DataShare data)
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