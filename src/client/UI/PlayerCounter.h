#ifndef PROJECT_PLAYERCOUNTER_H
#define PROJECT_PLAYERCOUNTER_H

#include "OnBoardObject.h"

/*
 *
 * RACE TO SPACE
 * The player counter is a visual representation of the player's current
 * position on the board.
 *
 */

class PlayerCounter : public OnBoardObject
{
 public:
  PlayerCounter() = default;
  ~PlayerCounter() = default;
};

#endif // PROJECT_PLAYERCOUNTER_H
