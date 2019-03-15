#ifndef PROJECT_SHIP_H
#define PROJECT_SHIP_H

#include "client/UI/OnBoardObject.h"
#include "gamelib/NetworkedData/Rooms.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <json.hpp>
using json = nlohmann::json;

class Ship
{
 public:
  Ship();
  ~Ship() = default;

  Vector2 getPositionForRoom(ship_rooms _room);

  void render();

 private:
  ASGE::Renderer* renderer;
  OnBoardObject board_sprite;
    json room_config;
};

#endif // PROJECT_SHIP_H
