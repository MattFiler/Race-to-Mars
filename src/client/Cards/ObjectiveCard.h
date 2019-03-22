#ifndef PROJECT_OBJECTIVECARD_H
#define PROJECT_OBJECTIVECARD_H

#include "Card.h"

/*
 *
 * RACE TO SPACE
 * Objective cards,given to the player on an interval around the board.
 *
 */

class ObjectiveCard : public Card
{
 public:
  void setCardID(objective_cards _obj_card_id);

 private:
  objective_cards m_cardID = objective_cards::OBJECTIVE_PLACEHOLDER
};

#endif // PROJECT_OBJECTIVECARD_H
