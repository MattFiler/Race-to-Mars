//
// Created by h2-sollis on 03/04/2019.
//

#ifndef PROJECT_ANIMATEDSPRITE_H
#define PROJECT_ANIMATEDSPRITE_H

#include "UI/ScaledSprite.h"
#include "client/Core/ServiceLocator.h"
#include "gamelib/Constants.h"
#include "gamelib/Math/BoundingBox.h"
#include "gamelib/Math/Vector2.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <string>

class AnimatedSprite
{
 public:
  AnimatedSprite();
  ~AnimatedSprite();

  void setNewWidth(float new_width);
  float getWidth() { return sprite->width(); };

  void setNewHeight(float new_height);
  float getHeight() { return sprite->height(); };

  void setNewLoc(Vector2 new_loc);
  Vector2 getLoc() { return Vector2(sprite->xPos(), sprite->yPos()); };

 private:
  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;
  BoundingBox bounding_box;
};

#endif // PROJECT_ANIMATEDSPRITE_H
