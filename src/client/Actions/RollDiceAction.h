//
// Created by j42-king on 22/03/2019.
//

#ifndef PROJECT_ROLLDICEACTION_H
#define PROJECT_ROLLDICEACTION_H

#include "Action.h"
#include <client/Players/Player.h>

class RollDiceAction : public Action
{
  Player player;

 public:
  RollDiceAction(Player& player1);

  void execute() override;
};

#endif // PROJECT_ROLLDICEACTION_H
