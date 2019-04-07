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

void ObjectiveCard::useObjectiveCard() {}

bool ObjectiveCard::objectiveComplete(IssueCard* _issue)
{
  IssueCard* issue = _issue;
  switch (cardID)
  {
    case objective_cards::BIG_ISSUE:
    {
      return issue->getActionPointsNeeded() == 15 &&
             issue->getAssignedPoints(static_cast<player_classes>(
               Locator::getPlayers()->my_player_index)) > 0;
    }
    case objective_cards::CLASS_ITEMS:
    {
      break;
    }
    case objective_cards::CLASS_SOLVER:
    {
      break;
    }
    case objective_cards::ISSUE_HELPER:
    {
      break;
    }
    case objective_cards::ISSUE_HELPER_15:
    {
      break;
    }
    case objective_cards::ISSUE_SOLVER_5:
    {
      break;
    }
    case objective_cards::ISSUE_SOLVER_10:
    {
      break;
    }
    case objective_cards::ISSUE_SOLVER_15:
    {
      break;
    }
    case objective_cards::MAJORITY_HELPER:
    {
      break;
    }
    case objective_cards::UNHELPFUL_RIGHT:
    {
      break;
    }
    case objective_cards::UNHELPFUL_LEFT:
    {
      break;
    }
    case objective_cards::PERSONAL_ISSUE:
    {
      break;
    }
    case objective_cards::POINT_STASH:
    {
      break;
    }
    default:
    {
      debug_text.print("No objective card exists of this type...");
      break;
    }
  }
  return false;
}
