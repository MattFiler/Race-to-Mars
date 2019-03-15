#ifndef PROJECT_BOUNDINGBOX_H
#define PROJECT_BOUNDINGBOX_H

#include "Vector2.h"

struct BoundingBox
{
  void setup(Vector2 _pos, float _width, float _height)
  {
    width = _width * GameResolution::scale;
    height = _height * GameResolution::scale;
    pos.update(_pos.x, _pos.y, false);
  }

  bool isInside(Vector2 _pos)
  {
    return (_pos.x > pos.x && _pos.x < pos.x + width) &&
           (_pos.y > pos.y && _pos.y < pos.y + height);
  }

  float width = 0;
  float height = 0;
  Vector2 pos;
};

#endif // PROJECT_BOUNDINGBOX_H
