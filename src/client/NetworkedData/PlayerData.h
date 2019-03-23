#ifndef PROJECT_PLAYERDATA_H
#define PROJECT_PLAYERDATA_H

#include "gamelib/NetworkedData/Players.h"

// All useful info for a player in the lobby
struct LobbyPlayer
{
  void performDisconnect()
  {
    has_connected = false;
    is_ready = false;
    is_this_client = false;
    is_active = false;
    current_class = player_classes::UNASSIGNED;
  }
  bool has_connected = false;
  bool is_this_client = false;
  bool is_ready = false;
  bool is_active = false;
  player_classes current_class = player_classes::UNASSIGNED;
};

#endif // PROJECT_PLAYERDATA_H
