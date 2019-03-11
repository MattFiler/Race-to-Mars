#ifndef PROJECT_COUNTER_H
#define PROJECT_COUNTER_H

#include "../Core/ServiceLocator.h"
#include <Engine/Renderer.h>

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

 private:
  ASGE::Renderer* renderer = nullptr;
};

#endif // PROJECT_COUNTER_H
