#ifndef PROJECT_SCALEDSPRITE_H
#define PROJECT_SCALEDSPRITE_H

#include "client/Core/ServiceLocator.h"
#include "gamelib/Constants.h"
#include "gamelib/Math/Vector2.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <string>

class ScaledSprite
{
 public:
  explicit ScaledSprite(const std::string& filename);
  ~ScaledSprite();

  void width(float width);
  float width() { return sprite->width(); };

  void height(float height);
  float height() { return sprite->height(); };

  void xPos(float x_pos);
  float xPos() { return sprite->xPos(); };

  void yPos(float y_pos);
  float yPos() { return sprite->yPos(); };

  void setPos(Vector2 new_pos);
  Vector2 getPos() { return Vector2(sprite->xPos(), sprite->yPos()); };

  ASGE::Sprite* getSprite() { return sprite; };

 private:
  ASGE::Renderer* renderer = nullptr;
  ASGE::Sprite* sprite = nullptr;
};

#endif // PROJECT_SCALEDSPRITE_H
