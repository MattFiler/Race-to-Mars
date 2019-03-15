#ifndef PROJECT_SHIPROOM_H
#define PROJECT_SHIPROOM_H

#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/Localisation/Localisation.h"
#include "gamelib/Math/BoundingBox.h"
#include "gamelib/Math/Vector2.h"
#include "gamelib/NetworkedData/Rooms.h"
#include <json.hpp>
using json = nlohmann::json;

class ShipRoom
{
 public:
  ShipRoom(const std::string& room_name, Vector2 board_offset);
  ~ShipRoom() = default;

  Vector2 getCentre();
  std::string getName();
  bool isInBoundingBox(Vector2 _pos);

 private:
  json room_config;
  FileHandler file_handler;
  BoundingBox bounding_box;
  Localisation localiser;
};

#endif // PROJECT_SHIPROOM_H
