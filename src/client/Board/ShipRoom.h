#ifndef PROJECT_SHIPROOM_H
#define PROJECT_SHIPROOM_H

#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/Localisation/Localisation.h"
#include "gamelib/Math/BoundingBox.h"
#include "gamelib/Math/Vector2.h"
#include "gamelib/NetworkedData/Players.h"
#include "gamelib/NetworkedData/Rooms.h"
#include <json.hpp>
using json = nlohmann::json;

class ShipRoom
{
 public:
  ShipRoom(const std::string& room_name,
           Vector2 board_offset,
           ship_rooms room_enum);
  ~ShipRoom() = default;

  Vector2 getPosForPlayer(player_classes _player);
  std::string getName();
  ship_rooms getEnum() { return this_room_enum; };
  bool isInBoundingBox(Vector2 _pos);
  bool canMoveTo(const std::string& room_name);

 private:
  FileHandler file_handler;
  BoundingBox bounding_box;
  Localisation localiser;

  std::vector<std::string> neighbour_rooms;
  ship_rooms this_room_enum;

  Vector2 centre_point;
  std::string friendly_name;
};

#endif // PROJECT_SHIPROOM_H
