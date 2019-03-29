#ifndef PROJECT_LOBBY_H
#define PROJECT_LOBBY_H

#include "gamelib/NetworkedData/Players.h"
#include <vector>

/* A game lobby */
struct Lobby
{
  Lobby() = delete;
  explicit Lobby(int _id) { lobby_id = _id; }
  int lobby_id = -1;

  /* Game client information */
  int user_ids[4] = { -1, -1, -1, -1 };
  bool users_ready[4] = { false, false, false, false };
  player_classes user_classes[4] = { player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED };
  int user_action_points[4] = { 0, 0, 0, 0 };

  /* Game card information */
  std::vector<int> issue_deck;
  std::vector<int> item_deck;
  std::vector<int> objective_deck;
  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };

  /* Game state information */
  int user_count = 0;
  int current_progress_index = 0;
  int player_that_started_id = -1;
  int currently_active_player = 0;
};

#endif // PROJECT_LOBBY_H
