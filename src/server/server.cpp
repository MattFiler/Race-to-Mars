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
  // chrono here
  //  std::random_device random;
  //  std::mt19937 mtgen(random(), random(), random());
  //  gen = mtgen;
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
  // Create deck of Issue IDS. 60 cards total.
  for (int i = 0; i < 2; ++i)
  {
    for (int j = 0; j < 30; ++j)
    {
      // Create for most recent lobby.
      lobbies.back().issue_deck.push_back(j);
    }
  }

  // Create deck of Item IDS. 38 cards total.
  for (int i = 0; i < 2; ++i)
  {
    for (int j = 0; j < 19; ++j)
    {
      // Create for most recent lobby.
      lobbies.back().item_deck.push_back(j);
    }
  }

  // Create deck of Objective IDS. 36 cards total.
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 12; ++j)
    {
      // Create for most recent lobby.
      lobbies.back().objective_deck.push_back(j);
    }
  }

  auto seed =
    std::chrono::high_resolution_clock::now().time_since_epoch().count();
  std::mt19937 gen(seed);
  // Shuffle decks.
  std::shuffle(
    lobbies.back().issue_deck.begin(), lobbies.back().issue_deck.end(), gen);
  std::shuffle(
    lobbies.back().item_deck.begin(), lobbies.back().item_deck.end(), gen);
  std::shuffle(lobbies.back().objective_deck.begin(),
               lobbies.back().objective_deck.end(),
               gen);

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
                   lobbies.at(client.lobby_index).players[0].class_type,
                   lobbies.at(client.lobby_index).players[1].class_type,
                   lobbies.at(client.lobby_index).players[2].class_type,
                   lobbies.at(client.lobby_index).players[3].class_type,
                   lobbies.at(client.lobby_index).players[0].is_ready,
                   lobbies.at(client.lobby_index).players[1].is_ready,
                   lobbies.at(client.lobby_index).players[2].is_ready,
                   lobbies.at(client.lobby_index).players[3].is_ready,
                   client.client_index);
          break;
        }

          // The currently active client wants to end their turn, so we need to
          // progress the game - this may be as simple as going to the next
          // player, but if we've done a full rotation in this lobby, new issue
          // cards will need to be pulled, etc
        case data_roles::CLIENT_WANTS_TO_END_TURN:
        {
          bool full_rotation = false;
          Lobby* this_clients_lobby = getLobbyByID(client.lobby_id);
          if (this_clients_lobby == nullptr)
          {
            break;
          }

          // Work out what client should go next
          if (this_clients_lobby->currently_active_player + 1 <=
              max_lobby_size - 1)
          {
            this_clients_lobby->currently_active_player++;
          }
          else
          {
            this_clients_lobby->currently_active_player = 0;
          }

          // If the next client was who started, we've done a "full rotation",
          // work out some game logic
          if (this_clients_lobby->currently_active_player ==
              this_clients_lobby->player_that_started_id)
          {
            full_rotation = true;

            // increment current ship position (win condition handling is done
            // client side)
            this_clients_lobby->current_progress_index++;

            // pull some new issue cards
            // FOR JACK TO DO!
            // GO THROUGH THE DECK HERE AND PICK OUT SOME ISSUE CARDS, ADD THEIR
            // IDs TO this_clients_lobby->active_issue_cards - WIN/LOSS
            // CONDITIONS SHOULD THEN BE HANDLED CLIENT SIDE
            int issues_tobe_drawn =
              amount_to_draw[this_clients_lobby->current_progress_index - 1];
            int issues_drawn = 0;

            for (int j = 0; j < 5; ++j)
            {
              // Check to see if slot is empty.
              if (issues_drawn < issues_tobe_drawn &&
                  this_clients_lobby->active_issue_cards[j] == -1)
              {
                this_clients_lobby->active_issue_cards[j] =
                  this_clients_lobby->issue_deck.back();
                debug_text.print(
                  "adding issue " +
                  std::to_string(this_clients_lobby->issue_deck.back()) +
                  " to active issues.");
                this_clients_lobby->issue_deck.pop_back();
                ++issues_drawn;
              }
            }
            // If this is an objective card spot, give a new objective card
            if (this_clients_lobby->current_progress_index % 3 == 0 &&
                this_clients_lobby->current_progress_index != 0)
            {
              // OBJECTIVE CARDS ARE A WIP!!
              for (int i = 0; i < 4; ++i)
              {
                this_clients_lobby->active_objective_cards[i] =
                  this_clients_lobby->objective_deck.back();
                debug_text.print(
                  "adding objective card " +
                  std::to_string(this_clients_lobby->objective_deck.back()) +
                  " to player: " + std::to_string(i));
                this_clients_lobby->objective_deck.pop_back();
              }
            }
          }

          // Forward current game data to all clients in this lobby
          sendData(client,
                   static_cast<unsigned int>(-1),
                   data_roles::SERVER_ENDED_CLIENT_TURN,
                   static_cast<int>(client.get_id()),
                   this_clients_lobby->currently_active_player,
                   this_clients_lobby->current_progress_index,
                   this_clients_lobby->active_issue_cards[0],
                   this_clients_lobby->active_issue_cards[1],
                   this_clients_lobby->active_issue_cards[2],
                   this_clients_lobby->active_issue_cards[3],
                   this_clients_lobby->active_issue_cards[4],
                   this_clients_lobby->active_objective_cards[0],
                   this_clients_lobby->active_objective_cards[1],
                   this_clients_lobby->active_objective_cards[2],
                   this_clients_lobby->active_objective_cards[3],
                   static_cast<int>(full_rotation));
          break;
        }

          // Sync lobby info to a player that joined the game in progress
        case data_roles::CLIENT_REQUESTS_SYNC:
        {
          Lobby* this_lobby = getLobbyByID(client.lobby_id);
          if (this_lobby == nullptr)
          {
            break;
          }

          // Sync information to that game
          sendData(client,
                   client.get_id(),
                   data_roles::SERVER_SYNCS_CARD_INFO,
                   this_lobby->active_issue_cards[0],
                   this_lobby->active_issue_cards[1],
                   this_lobby->active_issue_cards[2],
                   this_lobby->active_issue_cards[3],
                   this_lobby->active_issue_cards[4],
                   this_lobby->active_objective_cards[0],
                   this_lobby->active_objective_cards[1],
                   this_lobby->active_objective_cards[2],
                   this_lobby->active_objective_cards[3],
                   this_lobby->players[0].action_points,
                   this_lobby->players[1].action_points,
                   this_lobby->players[2].action_points,
                   this_lobby->players[3].action_points);
          sendData(client,
                   client.get_id(),
                   data_roles::SERVER_SYNCS_POSITION_INFO,
                   this_lobby->players[0].room_position,
                   this_lobby->players[1].room_position,
                   this_lobby->players[2].room_position,
                   this_lobby->players[3].room_position,
                   this_lobby->current_progress_index);
          break;
        }

          // Client spent or gained action points, update the lobby before
          // sending to all - we can use this data for reconnecting players to
          // keep them updated on game progress
        case data_roles::CLIENT_ACTION_POINTS_CHANGED:
        {
          Lobby* this_clients_lobby = getLobbyByID(client.lobby_id);
          if (this_clients_lobby == nullptr)
          {
            break;
          }

          // Update client's action point count for us
          this_clients_lobby->players[data_to_send.content[0]].action_points =
            data_to_send.content[1];
          debug_text.print(
            "Client " + std::to_string(data_to_send.content[0]) + " now has " +
            std::to_string(data_to_send.content[1]) + " action points.");

          goto SEND_TO_ALL; // Would rather not use goto, but there isn't much
                            // choice here.
        }

          // If a client is leaving a lobby, action that before letting the
          // lobby know
        case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
        {
          disconnectFromLobby(static_cast<int>(client.get_id()));
          goto SEND_TO_ALL;
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
          debug_text.print("Client " + std::to_string(data_to_send.content[0]) +
                           " is now in room " +
                           std::to_string(data_to_send.content[1]));
          this_lobby->players[data_to_send.content[0]].room_position =
            static_cast<ship_rooms>(data_to_send.content[1]);
          goto SEND_TO_ALL;
        }

          // We need to store lobby ready state before sending it out, so new
          // players are up to date
        case data_roles::CLIENT_CHANGED_LOBBY_READY_STATE:
        {
          bool did_send = false;
          Lobby* this_lobby = getLobbyByID(data_to_send.content[2]);
          if (this_lobby == nullptr)
          {
            break;
          }
          this_lobby->players[data_to_send.content[1]].is_ready =
            static_cast<bool>(data_to_send.content[0]);

          // See how many in the lobby are ready.
          int ready_count = 0;
          for (int i = 0; i < max_lobby_size; i++)
          {
            if (this_lobby->players[i].is_ready)
            {
              ready_count++;
            }
          }
          debug_text.print(std::to_string(ready_count) + " clients in lobby " +
                           std::to_string(this_lobby->lobby_id) +
                           " are ready to start.");

          // All clients are ready to start
          if (ready_count == max_lobby_size)
          {
            // If a game is already in progress, join it
            if (this_lobby->game_in_progress)
            {
              debug_text.print("Player is joining a game in progress in "
                               "lobby " +
                                 std::to_string(this_lobby->lobby_id) + ".",
                               -1);
              // Tell client to start the local game
              sendData(client,
                       static_cast<unsigned int>(-2),
                       data_roles::SERVER_STARTS_GAME,
                       this_lobby->currently_active_player,
                       1);
            }
            // Else, start a new game
            else
            {
              debug_text.print("Starting gameplay in lobby " +
                                 std::to_string(this_lobby->lobby_id) + ".",
                               -1);
              int starting_player = 0; // The id of the starting client, maybe
                                       // randomise this?
              this_lobby->game_in_progress = true;
              this_lobby->player_that_started_id = starting_player;
              this_lobby->currently_active_player = starting_player;
              did_send = true;
              sendData(client,
                       static_cast<unsigned int>(-1),
                       data_roles::SERVER_STARTS_GAME,
                       starting_player,
                       0);
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
          // the lobby.
        default:
        {
        SEND_TO_ALL:
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
                   data_to_send.content[9],
                   data_to_send.content[10],
                   data_to_send.content[11],
                   data_to_send.content[12],
                   data_to_send.content[13],
                   data_to_send.content[14]);
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
                                 int _content_10,
                                 int _content_11,
                                 int _content_12,
                                 int _content_13,
                                 int _content_14,
                                 int _content_15)
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
  data_to_send.content[10] = _content_11;
  data_to_send.content[11] = _content_12;
  data_to_send.content[12] = _content_13;
  data_to_send.content[13] = _content_14,
  data_to_send.content[14] = _content_15;
  packet_to_send << data_to_send;

  if (user_id == static_cast<unsigned int>(-1))
  {
    debug_text.print("Sending data to all clients in lobby " +
                     std::to_string(client.lobby_id) + ", of type " +
                     std::to_string(data_to_send.role) + ".");

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
        if (this_lobby.players[i].id == -1)
        {
          lobby_client_index = i; // find us an empty slot (someone may have
                                  // left, so we're not in sequence)
        }
      }
      this_lobby.players[lobby_client_index].id = client.get_id();
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
                       std::to_string(lobbies.size()) + " active lobbies.",
                     -1);

    lobby_client_index = 0;

    new_lobby.players[lobby_client_index].id = client.get_id();
    new_lobby.user_count++;
    client.lobby_id = new_lobby.lobby_id;
    client.lobby_index = static_cast<int>(lobbies.size() - 1);
  }

  // Now client is in a lobby, assign them a free class
  bool classes_taken[4] = { false, false, false, false };
  for (int i = 0; i < max_lobby_size; i++)
  {
    int taken_class = lobbies.at(client.lobby_index).players[i].class_type;
    if (taken_class != -1)
    {
      classes_taken[taken_class] = true;
    }
  }
  for (int i = 0; i < 4; i++)
  {
    if (!classes_taken[i])
    {
      lobbies.at(client.lobby_index).players[lobby_client_index].class_type =
        static_cast<player_classes>(i);
      break;
    }
  }
  client.client_index = lobby_client_index;

  debug_text.print(
    "Client " + std::to_string(client.get_id()) + " has joined lobby " +
    std::to_string(client.lobby_id) + " as role " +
    std::to_string(
      lobbies.at(client.lobby_index).players[lobby_client_index].class_type) +
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
      if (this_lobby.players[i].id == client_id)
      {
        // Disconnect from lobby
        this_lobby.user_count--;
        this_lobby.players[i].id = -1;
        this_lobby.players[i].is_ready = false;
        this_lobby.players[i].class_type = player_classes::UNASSIGNED;

        debug_text.print("Client " + std::to_string(client_id) +
                         " has left lobby " +
                         std::to_string(this_lobby.lobby_id) + ".");

        // If lobby is now empty, remove it so we can put new players in
        // populated lobbies
        if (this_lobby.user_count == 0)
        {
          lobbies.erase(lobbies.begin() + real_lobby_id);
          debug_text.print("Destroying unpopulated lobby. There are " +
                             std::to_string(lobbies.size()) +
                             " active lobbies.",
                           -1);
        }
      }
    }
    real_lobby_id++;
  }
}

// Find the client's actual lobby instance
Lobby* RaceToSpaceServer::getLobbyByID(int lobby_id)
{
  for (Lobby& this_lobby : lobbies)
  {
    if (this_lobby.lobby_id == lobby_id)
    {
      return &this_lobby;
    }
  }

  debug_text.print("ERROR - Couldn't retrieve lobby info for lobby " +
                     std::to_string(lobby_id) + "!",
                   2);
  return nullptr;
}