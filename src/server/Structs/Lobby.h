#ifndef PROJECT_LOBBY_H
#define PROJECT_LOBBY_H

#include "gamelib/NetworkedData/Players.h"

/* A game lobby */
struct Lobby
{
  Lobby() = delete;
  explicit Lobby(int _id) { lobby_id = _id; }
  int lobby_id = -1;
  int user_count = 0;
  int user_ids[4] = { -1, -1, -1, -1 };
  bool users_ready[4] = { false, false, false, false };
  player_classes user_classes[4] = { player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED };
  int current_progress_index = 0;
  int player_that_started_id = -1;
  int currently_active_player = 0;

  // BELOW IS ALL TEMP SHIT READY FOR JACK TO ADD HIS CARD IMPLEMENTATIONS
  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };
};

#endif // PROJECT_LOBBY_H
