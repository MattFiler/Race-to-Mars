#ifndef PROJECT_ISSUECARD_H
#define PROJECT_ISSUECARD_H

#include "Card.h"

/*
 *
 * RACE TO SPACE
 * Issue cards are dealt per whole group turn in quantities between 1-3. Five
 * total active issue cards ends the game.
 *
 */

class IssueCard : public Card
{
 public:
  void setCardID(issue_cards _item_card_id);

 private:
  issue_cards m_cardID = issue_cards::ISSUE_PLACEHOLDER;
};

#endif // PROJECT_ISSUECARD_H
