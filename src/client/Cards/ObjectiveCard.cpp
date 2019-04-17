#include "ObjectiveCard.h"
#include "IssueCard.h"
#include <client/Players/AllPlayers.h>

ObjectiveCard::ObjectiveCard(objective_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  auto card_type = static_cast<size_t>(_card_type);

  card_name = card_config["OBJECTIVECARDS"][card_type]["name"];
  cardID =
    static_cast<objective_cards>(card_config["OBJECTIVECARDS"][card_type]["card"
                                                                          "_i"
                                                                          "d"]);
  setSprite(card_config["OBJECTIVECARDS"][card_type]["sprite_path"]);
}

void ObjectiveCard::setCardID(objective_cards _obj_card_id)
{
  cardID = _obj_card_id;
}

void ObjectiveCard::useObjectiveCard()
{
  switch (cardID)
  {
    case objective_cards::BIG_ISSUE:
    {
    }
    case objective_cards::CLASS_ITEMS:
    {
    }
    case objective_cards::CLASS_SOLVER:
    {
    }
    case objective_cards::ISSUE_HELPER:
    {
    }
    case objective_cards::ISSUE_HELPER_15:
    {
    }
    case objective_cards::ISSUE_SOLVER_5:
    {
    }
    case objective_cards::ISSUE_SOLVER_10:
    {
    }
    case objective_cards::ISSUE_SOLVER_15:
    {
    }
    case objective_cards::MAJORITY_HELPER:
    {
    }
    case objective_cards::UNHELPFUL_RIGHT:
    {
    }
    case objective_cards::UNHELPFUL_LEFT:
    {
    }
    case objective_cards::PERSONAL_ISSUE:
    {
    }
    case objective_cards::POINT_STASH:
    {
    }
    default:
    {
      debug_text.print("No objective card exists of this type...");
      break;
    }
  }
}

bool ObjectiveCard::objectiveComplete(IssueCard* const _issue,
                                      player_classes _this_clients_class)
{
  IssueCard* issue = _issue;
  switch (cardID)
  {
    case objective_cards::BIG_ISSUE:
    {
      return (issue->getActionPointsNeeded() == 15 &&
              issue->getAssignedPoints(_this_clients_class) > 0);
    }
    case objective_cards::CLASS_ITEMS:
    {
      return (Locator::getPlayers()
                ->getPlayer(static_cast<player_classes>(
                  Locator::getPlayers()->my_player_index))
                ->getHeldItemAmount() >= 2);
    }
    case objective_cards::CLASS_SOLVER:
    {
      return (issue->getAssignedPoints(_this_clients_class) > 0 &&
              issue->isSolved() &&
              issue->getClassType() != _this_clients_class);
    }
    case objective_cards::ISSUE_HELPER:
    {
      // Player index doesnt map to class index. So must pass it in to get
      // correct class to check.
      return issue->getAssignedPoints(_this_clients_class) > 0;
    }
    case objective_cards::ISSUE_HELPER_15:
    {
      return (issue->getActionPointsNeeded() == 15 &&
              issue->getAssignedPoints(_this_clients_class) > 0);
    }
    case objective_cards::ISSUE_SOLVER_5:
    {
      return (issue->isSolvedSolo(_this_clients_class) &&
              issue->getActionPointsNeeded() == 5);
    }
    case objective_cards::ISSUE_SOLVER_10:
    {
      return (issue->isSolvedSolo(_this_clients_class) &&
              issue->getActionPointsNeeded() == 10);
    }
    case objective_cards::ISSUE_SOLVER_15:
    {
      return (issue->isSolvedSolo(_this_clients_class) &&
              issue->getActionPointsNeeded() == 15);
    }
    case objective_cards::MAJORITY_HELPER:
    {
      return (issue->isSolved() && issue->contributedMost(_this_clients_class));
    }
    case objective_cards::UNHELPFUL_RIGHT:
    {
      int right_player = 0;
      int this_player = Locator::getPlayers()->my_player_index;
      if (this_player + 1 > 3)
      {
        right_player = 0;
      }
      else
      {
        right_player = this_player + 1;
      }
      return (issue->getAssignedPoints(
                static_cast<player_classes>(right_player)) > 0 &&
              issue->getAssignedPoints(static_cast<player_classes>(
                Locator::getPlayers()->my_player_index)) == 0);
    }
    case objective_cards::UNHELPFUL_LEFT:
    {
      int left_player = 0;
      int this_player = Locator::getPlayers()->my_player_index;
      if (this_player - 1 < 0)
      {
        left_player = 3;
      }
      else
      {
        left_player = this_player - 1;
      }
      return (issue->getAssignedPoints(
                static_cast<player_classes>(left_player)) > 0 &&
              issue->getAssignedPoints(static_cast<player_classes>(
                Locator::getPlayers()->my_player_index)) == 0);
    }
    case objective_cards::PERSONAL_ISSUE:
    {
      return (issue->isSolved() &&
              issue->getClassType() == _this_clients_class);
    }
    case objective_cards::POINT_STASH:
    {
      return (Locator::getPlayers()
                ->getPlayer(_this_clients_class)
                ->getUsedApThisTurn());
    }
    default:
    {
      debug_text.print("No objective card exists of this type...");
      break;
    }
  }
  return false;
}
