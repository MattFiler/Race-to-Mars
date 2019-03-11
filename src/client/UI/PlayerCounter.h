#ifndef PROJECT_PLAYERCOUNTER_H
#define PROJECT_PLAYERCOUNTER_H

#include "Counter.h"

/*
 *
 * RACE TO SPACE
 * The player counter is a visual representation of the player's current
 * position on the board.
 *
 */

class PlayerCounter : Counter
{
 public:
  PlayerCounter() = default;
  ~PlayerCounter() = default;

  void setPosition(Vector2 _position) {
      position = _position;
  }

 private:
  Vector2 position;
};

#endif // PROJECT_PLAYERCOUNTER_H
