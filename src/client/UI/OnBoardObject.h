#ifndef PROJECT_COUNTER_H
#define PROJECT_COUNTER_H

#include "client/Core/ServiceLocator.h"
#include "client/UI/ScaledSprite.h"
#include "gamelib/Math/Vector2.h"
#include <Engine/Renderer.h>

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
  ScaledSprite* getSprite()
  {
    return sprite;
  } // shouldn't be required as we provide a render function, but will add the
    // functionality anyways for now

  void setPosition(Vector2 _position);
  Vector2 getPosition() { return position; }

  void setDimensions(Vector2 _dims);
  Vector2 getDimensions() { return dimensions; }

  void render();

 protected:
  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;

  Vector2 position = Vector2(0, 0);
  Vector2 dimensions = Vector2(0, 0);
};

#endif // PROJECT_COUNTER_H
