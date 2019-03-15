#ifndef PROJECT_SHIP_H
#define PROJECT_SHIP_H

#include "client/Board/ShipRoom.h"
#include "client/UI/OnBoardObject.h"
#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/NetworkedData/Rooms.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <json.hpp>
#include <vector>
using json = nlohmann::json;

class Ship
{
 public:
  Ship();
  ~Ship() = default;

  std::vector<ShipRoom> getRooms() { return rooms; };

  void render();

 private:
  ASGE::Renderer* renderer;
  OnBoardObject sprite;
  json ship_config;
  FileHandler file_handler;
  std::vector<ShipRoom> rooms;
  Vector2 base_size;
};

#endif // PROJECT_SHIP_H
