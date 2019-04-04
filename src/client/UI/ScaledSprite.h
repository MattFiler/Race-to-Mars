#ifndef PROJECT_SCALEDSPRITE_H
#define PROJECT_SCALEDSPRITE_H

#include "client/Core/ServiceLocator.h"
#include "gamelib/Constants.h"
#include "gamelib/Math/BoundingBox.h"
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

  void setDims(Vector2 dimensions);
  Vector2 getDims() { return Vector2(width(), height()); };

  void xPos(float x_pos);
  float xPos() { return sprite->xPos(); };

  void yPos(float y_pos);
  float yPos() { return sprite->yPos(); };

  // Temp show/hide - this is inefficient as we will still receive a render
  // call.
  void show() { sprite->opacity(1); };
  void hide() { sprite->opacity(0); };

  void setPos(Vector2 new_pos);
  Vector2 getPos() { return Vector2(sprite->xPos(), sprite->yPos()); };

  ASGE::Sprite* getSprite() { return sprite; };
  BoundingBox getBoundingBox() { return bounding_box; };

  // The scalar is used to tell the difference between the previous size and
  // newly set size.
  float getScalar() { return scalar; };

 private:
  ASGE::Renderer* renderer = nullptr;
  ASGE::Sprite* sprite = nullptr;
  BoundingBox bounding_box;
  float scalar = 1.0f;
};

#endif // PROJECT_SCALEDSPRITE_H
