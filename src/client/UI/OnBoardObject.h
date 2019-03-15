#ifndef PROJECT_COUNTER_H
#define PROJECT_COUNTER_H

#include "client/Core/ServiceLocator.h"
#include "client/Math/Vector2.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>

/*
 *
 * RACE TO SPACE
 * An object on the board - E.G. a player, a counter, the ship.
 *
 */

class OnBoardObject
{
 public:
  OnBoardObject();
  ~OnBoardObject() = default;

  void setSprite(const std::string& sprite_path);

  void setPosition(Vector2 _position);
  Vector2 getPosition() { return position; }

  void setDimensions(Vector2 _dims);
  Vector2 getDimensions() { return dimensions; }

  void render();

 protected:
  ASGE::Renderer* renderer = nullptr;
  ASGE::Sprite* sprite = nullptr;

  Vector2 position = Vector2(0, 0);
  Vector2 dimensions = Vector2(0, 0);
};

#endif // PROJECT_COUNTER_H
