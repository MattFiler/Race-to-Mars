#ifndef PROJECT_PLAYER_H
#define PROJECT_PLAYER_H

#include "../Core/ServiceLocator.h"
#include <Engine/Renderer.h>

/*
 *
 * RACE TO SPACE
 * An instance of our generic player, to be used as a parent to all classes.
 *
 */

class Player
{
 public:
  Player();
  ~Player() = default;

 protected:
  ASGE::Renderer* renderer = nullptr;

 private:
  /* Stuff here */
};

#endif // PROJECT_PLAYER_H
