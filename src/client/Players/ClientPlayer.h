#ifndef PROJECT_CLIENTPLAYER_H
#define PROJECT_CLIENTPLAYER_H

#include "gamelib/NetworkedData/Players.h"

/*
 *
 * This class forms information about the client and their current role in the
 * game.
 *
 */

class ClientPlayer
{
 public:
  ClientPlayer() = default;
  ~ClientPlayer() = default;

  void SetActive(bool _is_my_go) { currently_my_go = _is_my_go; };
  bool GetActive() { return currently_my_go; };

  void SetMyClass(player_classes _my_class) { my_class = _my_class; };
  player_classes GetMyClass() { return my_class; };

 private:
  bool currently_my_go = false;
  player_classes my_class = player_classes::UNASSIGNED;
};

#endif // PROJECT_CLIENTPLAYER_H
