#include "server.h"

/*
 *
 * This is still a way away from the tidyup that the server needs, but it's a
 * start.
 *
 * To go further with the tidyup, it would be nice to get cards into their own
 * class within the server instead of all being in the same place.
 *
 */

/* Handle data received by the server */
void RaceToSpaceServer::handleReceivedData(DataShare& data_to_send,
                                           server_client& client)
{
  switch (data_to_send.getType())
  {
    // If client requests lobby info, we need to send that directly to the
    // client that wants it.
    case data_roles::CLIENT_REQUESTS_TO_JOIN_LOBBY:
    {
      clientJoinLobby(client);
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
      clientPointsChange(data_to_send, client);
      break;
    }

    // If a client is leaving a lobby, action that before letting the
    // lobby know
    case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
    {
      disconnectFromLobby(static_cast<int>(client.get_id()));
      sendData(client, static_cast<unsigned int>(-1), data_to_send);
      break;
    }

    // If client moves in the ship, save that first for syncing to
    // reconnecting players
    case data_roles::CLIENT_MOVING_PLAYER_TOKEN:
    {
      clientMoved(data_to_send, client);
      break;
    }

    // Updates the new progress index for all clients after dice roll
    // issue event on client.
    case data_roles::CLIENT_CHANGE_PROGRESS_INDEX:
    {
      clientProgressChange(data_to_send, client);
      break;
    }

    // Client has requested to buy an item.
    case data_roles::CLIENT_REQUESTED_ITEM_CARD:
    {
      clientRequestsItem(data_to_send, client);
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
      sendData(client, static_cast<unsigned int>(-1), data_to_send);
      break;
    }
  }
}

/* End the turn for the active player in lobby */
void RaceToSpaceServer::endTurn(server_client& client)
{
  bool full_rotation = false;
  Lobby* this_clients_lobby = getLobbyByID(client.lobby_id);
  if (this_clients_lobby == nullptr)
  {
    return;
  }
  // Work out what client should go next
  if (this_clients_lobby->currently_active_player + 1 <= max_lobby_size - 1)
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
        debug_text.print("adding issue " +
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

  // Compile data to share out
  DataShare new_share = DataShare(data_roles::SERVER_ENDED_CLIENT_TURN);
  new_share.add(static_cast<int>(client.get_id()));
  new_share.add(this_clients_lobby->currently_active_player);
  new_share.add(this_clients_lobby->current_progress_index);
  new_share.add(this_clients_lobby->active_issue_cards[0]);
  new_share.add(this_clients_lobby->active_issue_cards[1]);
  new_share.add(this_clients_lobby->active_issue_cards[2]);
  new_share.add(this_clients_lobby->active_issue_cards[3]);
  new_share.add(this_clients_lobby->active_issue_cards[4]);
  new_share.add(this_clients_lobby->active_objective_cards[0]);
  new_share.add(this_clients_lobby->active_objective_cards[1]);
  new_share.add(this_clients_lobby->active_objective_cards[2]);
  new_share.add(this_clients_lobby->active_objective_cards[3]);
  new_share.add(static_cast<int>(full_rotation));

  // Forward current game data to all clients in this lobby
  sendData(client, static_cast<unsigned int>(-1), new_share);
}

/* Sync a new client to get them up to speed with the game in progress */
void RaceToSpaceServer::syncClient(server_client& client)
{
  Lobby* this_lobby = getLobbyByID(client.lobby_id);
  if (this_lobby == nullptr)
  {
    return;
  }

  // Compile data PT 1 (this can probs be condensed to just one packet
  // now we have our new class
  DataShare new_share = DataShare(data_roles::SERVER_SYNCS_CARD_INFO);
  new_share.add(this_lobby->active_issue_cards[0]);
  new_share.add(this_lobby->active_issue_cards[1]);
  new_share.add(this_lobby->active_issue_cards[2]);
  new_share.add(this_lobby->active_issue_cards[3]);
  new_share.add(this_lobby->active_issue_cards[4]);
  new_share.add(this_lobby->active_objective_cards[0]);
  new_share.add(this_lobby->active_objective_cards[1]);
  new_share.add(this_lobby->active_objective_cards[2]);
  new_share.add(this_lobby->active_objective_cards[3]);
  new_share.add(this_lobby->players[0].action_points);
  new_share.add(this_lobby->players[1].action_points);
  new_share.add(this_lobby->players[2].action_points);
  new_share.add(this_lobby->players[3].action_points);

  // Compile data PT 2 (yeah really could be easily refactored!)
  DataShare new_share_2 = DataShare(data_roles::SERVER_SYNCS_POSITION_INFO);
  new_share_2.add(this_lobby->players[0].room_position);
  new_share_2.add(this_lobby->players[1].room_position);
  new_share_2.add(this_lobby->players[2].room_position);
  new_share_2.add(this_lobby->players[3].room_position);
  new_share_2.add(this_lobby->current_progress_index);

  // Sync information to that game
  sendData(client, client.get_id(), new_share);
  sendData(client, client.get_id(), new_share_2);
}

/* A client readied up, do we need to start the game? */
void RaceToSpaceServer::clientReadyUp(DataShare& data_to_send,
                                      server_client& client)
{
  bool did_send = false;
  Lobby* this_lobby = getLobbyByID(data_to_send.retrieve(2));
  if (this_lobby == nullptr)
  {
    return;
  }
  this_lobby->players[data_to_send.retrieve(1)].is_ready =
    static_cast<bool>(data_to_send.retrieve(0));

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
      DataShare new_share = DataShare(data_roles::SERVER_STARTS_GAME);
      new_share.add(this_lobby->currently_active_player);
      new_share.add(1);
      sendData(client, static_cast<unsigned int>(-2), new_share);
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
      DataShare new_share = DataShare(data_roles::SERVER_STARTS_GAME);
      new_share.add(starting_player);
      new_share.add(0);
      sendData(client, static_cast<unsigned int>(-1), new_share);
    }
  }
  if (!did_send)
  {
    sendToAll(client, data_to_send);
  }
}

/* Connect to a lobby in the server */
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
    // create and shuffle decks here. create a function which calls here.
    initLobbyDecks();
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

/* Take client out of the lobby they were registered to */
void RaceToSpaceServer::disconnectFromLobby(int client_id)
{
  int real_lobby_id = 0;
  for (Lobby& this_lobby : lobbies)
  {
    for (int i = 0; i < max_lobby_size; i++)
    {
      if (this_lobby.players[i].id != client_id)
      {
        continue;
      }

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
                           std::to_string(lobbies.size()) + " active lobbies.",
                         -1);
      }
    }
    real_lobby_id++;
  }
}

/* Find the client's actual lobby instance */
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

/* Initialise the card decks */
void RaceToSpaceServer::initLobbyDecks()
{
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
}

/* Client needs to join a lobby */
void RaceToSpaceServer::clientJoinLobby(server_client& client)
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
}

/* Client's action points changed */
void RaceToSpaceServer::clientPointsChange(DataShare& data_to_send,
                                           server_client& client)
{
  Lobby* this_clients_lobby = getLobbyByID(client.lobby_id);
  if (this_clients_lobby == nullptr)
  {
    return;
  }

  // Update client's action point count for us
  this_clients_lobby->players[data_to_send.retrieve(0)].action_points =
    data_to_send.retrieve(1);
  debug_text.print("Client " + std::to_string(data_to_send.retrieve(0)) +
                   " now has " + std::to_string(data_to_send.retrieve(1)) +
                   " action points.");

  sendToAll(client, data_to_send);
}

/* Client moved to new position */
void RaceToSpaceServer::clientMoved(DataShare& data_to_send,
                                    server_client& client)
{
  Lobby* this_lobby = getLobbyByID(client.lobby_id);
  if (this_lobby == nullptr)
  {
    return;
  }
  debug_text.print("Client " + std::to_string(data_to_send.retrieve(0)) +
                   " is now in room " +
                   std::to_string(data_to_send.retrieve(1)));
  this_lobby->players[data_to_send.retrieve(0)].room_position =
    static_cast<ship_rooms>(data_to_send.retrieve(1));

  sendToAll(client, data_to_send);
}

/* Client updated game progress */
void RaceToSpaceServer::clientProgressChange(DataShare& data_to_send,
                                             server_client& client)
{
  Lobby* this_lobby = getLobbyByID(client.lobby_id);
  if (this_lobby == nullptr)
  {
    return;
  }
  debug_text.print("Changing progress from: " +
                   std::to_string(this_lobby->current_progress_index) +
                   " to:" + std::to_string(data_to_send.retrieve(0)));
  this_lobby->current_progress_index = data_to_send.retrieve(0);

  // Compile and send it on
  DataShare new_share = DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
  new_share.add(data_to_send.retrieve(0));
  sendData(client, static_cast<unsigned int>(-2), new_share);
}

/* Client requests an item card */
void RaceToSpaceServer::clientRequestsItem(DataShare& data_to_send,
                                           server_client& client)
{
  Lobby* this_lobby = getLobbyByID(client.lobby_id);
  if (this_lobby == nullptr)
  {
    return;
  }

  // Compile data and send it back off
  DataShare new_share = DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
  new_share.add(data_to_send.retrieve(0));
  new_share.add(this_lobby->item_deck.back());
  this_lobby->item_deck.pop_back();

  sendData(client, static_cast<unsigned int>(-2), new_share);
}