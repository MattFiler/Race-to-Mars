#ifndef PROJECT_COUNTER_H
#define PROJECT_COUNTER_H

#include "client/Core/ServiceLocator.h"
#include "client/UI/ScaledSprite.h"
#include "gamelib/Constants.h"
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
  ScaledSprite* getSprite() { return sprite; }

  void setShadowSprite(const std::string& sprite_path);
  ScaledSprite* getShadowSprite() { return sprite_shadow; };

  void setPosition(Vector2 _position, bool force = false);
  Vector2 getPosition() { return position; }

  void setDimensions(Vector2 _dims, bool force = false);
  Vector2 getDimensions() { return dimensions; }

  bool isInBoundingBox(Vector2 _pos);

  void render(render_order _order);

 protected:
  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;

  ScaledSprite* sprite_shadow = nullptr;
  bool has_shadow = false;
  float shadow_scalar = 1.0f;

  CardOffsets card_offsets;

  Vector2 position = Vector2(0, 0);
  Vector2 dimensions = Vector2(0, 0);
};

#endif // PROJECT_COUNTER_H
