#include "ObjectiveCard.h"
#include "IssueCard.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include <client/Players/AllPlayers.h>
#include <gamelib/NetworkedData/DataShare.h>

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
      // Free ship movement
      freeShipMovement();
      break;
    }
    case objective_cards::CLASS_ITEMS:
    {
      // Swap current items for new ones.
      Locator::getPlayers()
        ->getPlayer(
          static_cast<player_classes>(Locator::getPlayers()->my_player_index))
        ->setReplenishItems(true);
      break;
    }
    case objective_cards::CLASS_SOLVER:
    {
      // 3 + ap
      increaseAP();
      break;
    }
    case objective_cards::ISSUE_HELPER:
    {
      //+3 AP
      increaseAP();
      break;
    }
    case objective_cards::ISSUE_HELPER_15:
    {
      // Delete an issue immediately
      increaseAP();
      break;
    }
    case objective_cards::ISSUE_SOLVER_5:
    {
      //+3 AP's'
      increaseAP();
      break;
    }
    case objective_cards::ISSUE_SOLVER_10:
    {
      // Free ship movement
      freeShipMovement();
      break;
    }
    case objective_cards::ISSUE_SOLVER_15:
    {
      // Move ship forward
      DataShare new_share = DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
      new_share.add(Locator::getPlayers()->current_progress_index + 1);
      Locator::getNetworkInterface()->sendData(new_share);
      break;
    }
    case objective_cards::MAJORITY_HELPER:
    {
      // Draw item card
      if (Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->getHeldItemAmount() < Locator::getPlayers()
                                      ->getPlayer(static_cast<player_classes>(
                                        Locator::getPlayers()->my_player_index))
                                      ->getMaxItems())
      {
        DataShare new_share_item =
          DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
        new_share_item.add(Locator::getPlayers()->my_player_index);
        Locator::getNetworkInterface()->sendData(new_share_item);
      }
      break;
    }
    case objective_cards::UNHELPFUL_RIGHT:
    {
      // Draw item card.
      if (Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->getHeldItemAmount() < Locator::getPlayers()
                                      ->getPlayer(static_cast<player_classes>(
                                        Locator::getPlayers()->my_player_index))
                                      ->getMaxItems())
      {
        DataShare new_share_item =
          DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
        new_share_item.add(Locator::getPlayers()->my_player_index);
        Locator::getNetworkInterface()->sendData(new_share_item);
      }
      break;
    }
    case objective_cards::UNHELPFUL_LEFT:
    {
      // Re-roll the dice.
      Locator::getPlayers()
        ->getPlayer(
          static_cast<player_classes>(Locator::getPlayers()->my_player_index))
        ->setDiceRolls(1);
      break;
    }
    case objective_cards::PERSONAL_ISSUE:
    {
      //+3 ap
      increaseAP();
      break;
    }
    case objective_cards::POINT_STASH:
    {
      // Double your action points.
      int& my_action_points =
        Locator::getPlayers()
          ->players[Locator::getPlayers()->my_player_index]
          .action_points;
      DataShare new_share = DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
      new_share.add(Locator::getPlayers()->my_player_index);
      new_share.add(my_action_points);
      new_share.add(my_action_points * 2);
      new_share.add(-1);
      Locator::getNetworkInterface()->sendData(new_share);
      break;
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

void ObjectiveCard::increaseAP()
{
  int& my_action_points = Locator::getPlayers()
                            ->players[Locator::getPlayers()->my_player_index]
                            .action_points;
  DataShare new_share = DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
  new_share.add(Locator::getPlayers()->my_player_index);
  new_share.add(my_action_points);
  new_share.add(my_action_points + 3);
  new_share.add(-1);
  Locator::getNetworkInterface()->sendData(new_share);
}

void ObjectiveCard::freeShipMovement()
{
  DataShare new_share = DataShare(data_roles::CLIENT_FREE_MOVEMENT);
  new_share.add(true);
  Locator::getNetworkInterface()->sendData(new_share);
}
