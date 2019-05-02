#ifndef PROJECT_CURSOR_H
#define PROJECT_CURSOR_H

#include "client/UI/ScaledSprite.h"
#include "gamelib/Constants.h"
#include "gamelib/Math/Vector2.h"
#include <Engine/Renderer.h>
#include <memory>

class Cursor
{
 public:
  Cursor() = default;
  ~Cursor();
  void configure();

  void setCursorActive(bool _active);

  void updatePosition(double x_pos, double y_pos);
  Vector2 getPosition();

  void render();

 private:
  ScaledSprite* cursor_sprite_active = nullptr;
  ScaledSprite* cursor_sprite_inactive = nullptr;
  ASGE::Renderer* renderer = nullptr;
  bool is_active = false;
  Vector2 cursor_pos;
};

#endif // PROJECT_CURSOR_H
