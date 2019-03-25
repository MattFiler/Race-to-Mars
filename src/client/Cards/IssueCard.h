#ifndef PROJECT_ISSUECARD_H
#define PROJECT_ISSUECARD_H

#include "Card.h"
#include <client/Actions/Action.h>
#include <gamelib/NetworkedData/Players.h>

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
  void addActionPoints(player_classes _player_class, int _ap_amount);
  issue_cards getCardID();
  bool isSolved() { return issue_solved; }
  Action* m_issue_event = nullptr;

 private:
  issue_cards m_cardID = issue_cards::ISSUE_PLACEHOLDER;

  int m_comms_ap_assigned = 0;
  int m_engineer_ap_assigned = 0;
  int m_medic_ap_assigned = 0;
  int m_pilot_ap_assigned = 0;
  int m_total_ap_assigned = 0;

  bool issue_solved = false;
};

#endif // PROJECT_ISSUECARD_H
