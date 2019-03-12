#ifndef PROJECT_PLAYERDATA_H
#define PROJECT_PLAYERDATA_H

#include "gamelib/Networked/Cards.h"
#include "gamelib/Networked/Players.h"
#include "gamelib/Networked/Rooms.h"

struct PlayerData
{
  int action_points;
  ship_rooms current_room;
  objective_cards current_objective;
  player_classes class_identifier;
};

#endif // PROJECT_PLAYERDATA_H
