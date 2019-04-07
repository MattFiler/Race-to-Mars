#ifndef PROJECT_OBJECTIVECARD_H
#define PROJECT_OBJECTIVECARD_H

#include "Card.h"

/*
 *
 * RACE TO SPACE
 * Objective cards,given to the player on an interval around the board.
 *
 */

class IssueCard;

class ObjectiveCard : public Card
{
 public:
  explicit ObjectiveCard(objective_cards _card_type);
  ~ObjectiveCard() = default;

  void setCardID(objective_cards _obj_card_id);
  objective_cards getCardID() { return cardID; };

  void useObjectiveCard();
  bool objectiveComplete(IssueCard* _issue);

 private:
  objective_cards cardID = objective_cards::OBJECTIVE_PLACEHOLDER;
};

#endif // PROJECT_OBJECTIVECARD_H
