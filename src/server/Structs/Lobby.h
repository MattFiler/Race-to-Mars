#ifndef PROJECT_LOBBY_H
#define PROJECT_LOBBY_H

#include "gamelib/NetworkedData/Players.h"
#include "gamelib/NetworkedData/Rooms.h"
#include <vector>

struct PlayerInLobby
{
  int id = -1;
  bool is_ready = false;
  player_classes class_type = player_classes::UNASSIGNED;
  int action_points = 0;
  ship_rooms room_position = ship_rooms::SUPPLY_BAY; // this is properly
                                                     // initialised later
};

/* A game lobby */
struct Lobby
{
  Lobby() = delete;
  explicit Lobby(int _id) { lobby_id = _id; }
  int lobby_id = -1;

  /* Game client information */
  PlayerInLobby players[4];

  /* Game card information */
  std::vector<int> issue_deck;
  std::vector<int> item_deck;
  std::vector<int> objective_deck;
  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };
  int active_objective_cards[4] = { -1, -1, -1, -1 };

  /* Game state information */
  bool game_in_progress = false;
  int user_count = 0;
  int current_progress_index = 0;
  int player_that_started_id = -1;
  int currently_active_player = 0;
};

#endif // PROJECT_LOBBY_H
