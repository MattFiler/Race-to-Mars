#include "ObjectiveCard.h"
#include "IssueCard.h"
#include <client/Players/AllPlayers.h>

ObjectiveCard::ObjectiveCard(objective_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  auto card_type = static_cast<size_t>(_card_type);

  card_name = card_config["OBJECTIVECARDS"][card_type]["name"];
  // m_card_decription =
  // card_config["OBJECTIVECARDS"][card_type]["description"];
  // m_action_points =
  // card_config["OBJECTIVECARDS"][card_type]["action_points"];
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

bool ObjectiveCard::objectiveComplete(IssueCard* const _issue)
{
  // IssueCard* issue = _issue;
  switch (cardID)
  {
    case objective_cards::BIG_ISSUE:
    {
      //      bool completed = issue->getActionPointsNeeded() == 15 &&
      //                       issue->getAssignedPoints(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) > 0;
      //      return completed;
      return true;
    }
    case objective_cards::CLASS_ITEMS:
    {
      //      bool completed = Locator::getPlayers()
      //                         ->getPlayer(static_cast<player_classes>(
      //                           Locator::getPlayers()->my_player_index))
      //                         ->getHeldItemAmount() >= 2;
      //      return completed;
      return true;
    }
    case objective_cards::CLASS_SOLVER:
    {
      //      bool completed =
      //        issue->getAssignedPoints(static_cast<player_classes>(
      //          Locator::getPlayers()->my_player_index)) > 0 &&
      //        issue->isSolved() &&
      //        issue->getClassType() !=
      //          static_cast<player_classes>(Locator::getPlayers()->my_player_index);
      //      return completed;
      return true;
    }
    case objective_cards::ISSUE_HELPER:
    {
      //      bool completed =
      //      issue->getAssignedPoints(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) > 0;
      //      return completed;
      return true;
    }
    case objective_cards::ISSUE_HELPER_15:
    {
      //      bool completed = issue->getActionPointsNeeded() == 15 &&
      //                       issue->getAssignedPoints(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) > 0;
      //      return completed;
      return true;
    }
    case objective_cards::ISSUE_SOLVER_5:
    {
      //      bool completed = issue->isSolvedSolo(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) &&
      //                       issue->getActionPointsNeeded() == 5;
      //      return completed;
      return true;
    }
    case objective_cards::ISSUE_SOLVER_10:
    {
      //      bool completed = issue->isSolvedSolo(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) &&
      //                       issue->getActionPointsNeeded() == 10;
      //      return completed;
      return true;
    }
    case objective_cards::ISSUE_SOLVER_15:
    {
      //      bool completed = issue->isSolvedSolo(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) &&
      //                       issue->getActionPointsNeeded() == 15;
      //      return completed;
      return true;
    }
    case objective_cards::MAJORITY_HELPER:
    {
      //      bool completed = issue->isSolved() &&
      //                       issue->contributedMost(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index));
      //      return completed;
      return true;
    }
    case objective_cards::UNHELPFUL_RIGHT:
    {
      //      int right_player = 0;
      //      int this_player = Locator::getPlayers()->my_player_index;
      //      if (this_player + 1 > 3)
      //      {
      //        right_player = 0;
      //      }
      //      else
      //      {
      //        right_player = this_player + 1;
      //      }
      //      bool completed = issue->getAssignedPoints(
      //                         static_cast<player_classes>(right_player)) > 0
      //                         &&
      //                       issue->getAssignedPoints(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) == 0;
      //      return completed;
      return true;
    }
    case objective_cards::UNHELPFUL_LEFT:
    {
      //      int left_player = 0;
      //      int this_player = Locator::getPlayers()->my_player_index;
      //      if (this_player - 1 < 0)
      //      {
      //        left_player = 0;
      //      }
      //      else
      //      {
      //        left_player = this_player - 1;
      //      }
      //      bool completed = issue->getAssignedPoints(
      //                         static_cast<player_classes>(left_player)) > 0
      //                         &&
      //                       issue->getAssignedPoints(static_cast<player_classes>(
      //                         Locator::getPlayers()->my_player_index)) == 0;
      //      return completed;
      return true;
    }
    case objective_cards::PERSONAL_ISSUE:
    {
      //      bool completed =
      //        issue->isSolved() &&
      //        issue->getClassType() ==
      //          static_cast<player_classes>(Locator::getPlayers()->my_player_index);
      //      return completed;
      return true;
    }
    case objective_cards::POINT_STASH:
    {
      //      bool completed = Locator::getPlayers()
      //                         ->getPlayer(static_cast<player_classes>(
      //                           Locator::getPlayers()->my_player_index))
      //                         ->getUsedApThisTurn();
      //      return completed;
      return true;
    }
    default:
    {
      debug_text.print("No objective card exists of this type...");
      break;
    }
  }
  return false;
}
