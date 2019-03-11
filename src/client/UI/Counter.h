#ifndef PROJECT_COUNTER_H
#define PROJECT_COUNTER_H

#include "../Core/ServiceLocator.h"
#include "../Math/Vector2.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>

/*
 *
 * RACE TO SPACE
 * A counter to represent the position of something on our game board - E.G. a
 * player or the ship.
 *
 */

class Counter
{
 public:
  Counter();
  ~Counter() = default;

  void setSprite(const std::string& sprite_path);

  void setPosition(Vector2 _position);
  Vector2 getPosition() { return position; }

  void render();

 protected:
  ASGE::Renderer* renderer = nullptr;
  ASGE::Sprite* sprite = nullptr;

  Vector2 position = Vector2(0, 0);
};

#endif // PROJECT_COUNTER_H
