#ifndef PROJECT_SHIP_H
#define PROJECT_SHIP_H

#include "client/UI/OnBoardObject.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>

class Ship
{
 public:
  Ship();
  ~Ship() = default;

  void render();

 private:
  ASGE::Renderer* renderer;
  OnBoardObject board_sprite;
};

#endif // PROJECT_SHIP_H
