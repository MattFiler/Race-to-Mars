//
// Created by j42-king on 22/03/2019.
//

#include "RollDiceAction.h"

RollDiceAction::RollDiceAction(Player& player1) : player(player1) {}

void RollDiceAction::execute()
{
  // call player.rolldice function here.
  // set amount added.
}

void RollDiceAction::unExecute()
{
  // for whatever reason remove the previous dice roll.
}
