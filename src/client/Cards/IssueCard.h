#ifndef PROJECT_ISSUECARD_H
#define PROJECT_ISSUECARD_H

#include "Card.h"
#include "gamelib/Constants.h"
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
  explicit IssueCard(issue_cards _card_type);
  ~IssueCard() = default;
  void setCardID(issue_cards _item_card_id);
  issue_cards getCardID() { return cardID; };
  void addActionPoints(player_classes _player_class, int _ap_amount);
  void setIssueCardvariable(int _action_points);

  int getAssignedPointsGlobal() { return total_ap_assigned; };
  int getAssignedPoints(player_classes _class);

  bool isSolved();

 private:
  issue_cards cardID = issue_cards::ISSUE_PLACEHOLDER;
  int comms_ap_assigned = 0;
  int engineer_ap_assigned = 0;
  int medic_ap_assigned = 0;
  int pilot_ap_assigned = 0;
  int total_ap_assigned = 0;
  bool issue_solved = false;

  int issue_card_ap_variable = 0;
};

#endif // PROJECT_ISSUECARD_H
