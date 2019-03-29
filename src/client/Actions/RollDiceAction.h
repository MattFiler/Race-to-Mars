//
// Created by j42-king on 22/03/2019.
//

#ifndef PROJECT_ROLLDICEACTION_H
#define PROJECT_ROLLDICEACTION_H

#include "Action.h"
#include <client/Players/Player.h>

class RollDiceAction : public Action
{
 public:
  explicit RollDiceAction(Player& player1);

  void execute() override;
  void unExecute();

 private:
  Player& player;
};
#endif // PROJECT_ROLLDICEACTION_H
