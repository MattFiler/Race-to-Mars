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

  void createSprite(const std::string& name)
  {
    sprite = new ScaledSprite(name);
  }

  float getWidth() { return sprite->width(); };
  void setNewWidth(float new_width);

  float getHeight() { return sprite->height(); };
  void setNewHeight(float new_height);

  float getMoveSpeed() { return moveSpeed; };
  void setMoveSpeed(float speed);

  Vector2 getLoc() { return Vector2(sprite->xPos(), sprite->yPos()); };
  void setNewLoc(Vector2 new_loc);

  // void setOpacity

 private:
  float moveSpeed = 300;

  ASGE::Renderer* renderer = nullptr;
  ScaledSprite* sprite = nullptr;
  BoundingBox bounding_box;
};

#endif // PROJECT_ANIMATEDSPRITE_H
