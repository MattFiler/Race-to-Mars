#include "GameBoard.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "gamelib/Constants.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include <client/Cards/ObjectiveCard.h>

/* Check to see if we're hovering over an interactable room  */
bool GameBoard::isHoveringOverRoom(Vector2 hover_pos)
{
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(hover_pos))
    {
      return true;
    }
  }
  return false;
}

/* Check to see if we're hovering over an issue card  */
bool GameBoard::isHoveringOverIssueCard(Vector2 hover_pos)
{
  for (IssueCard& card : active_issues)
  {
    if (card.isInBoundingBox(hover_pos))
    {
      return true;
    }
  }
  return false;
}

/* Check to see if we're hovering over an objective card  */
bool GameBoard::isHoveringOverObjectiveCard(Vector2 hover_pos)
{
  return (active_obj_card != nullptr &&
          active_obj_card->isInBoundingBox(hover_pos));
}

/* Return clicked interactable room  */
ShipRoom GameBoard::getClickedRoom(Vector2 clicked_pos)
{
  // Check rooms
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(clicked_pos))
    {
      return room;
    }
  }

  // Didn't match any
  throw "This function must be called based on the result of "
        "isHoveringOverRoom.";
}

/* Return clicked issue card  */
IssueCard* GameBoard::getClickedIssueCard(Vector2 clicked_pos)
{
  // Check issue card deck
  for (IssueCard& card : active_issues)
  {
    if (card.isInBoundingBox(clicked_pos))
    {
      return &card;
    }
  }

  // Didn't match any
  throw "This function must be called based on the result of "
        "isHoveringOverIssueCard.";
}

/* Return clicked objective card  */
ObjectiveCard* GameBoard::getClickedObjectiveCard(Vector2 clicked_pos)
{
  // Check objective card
  if (active_obj_card->isInBoundingBox(clicked_pos))
  {
    return active_obj_card;
  }

  // Didn't match any
  throw "This function must be called based on the result of "
        "isHoveringOverObjectiveCard.";
}

/* Set the active objective card to update */
void GameBoard::setActiveObjectiveCard(int card_index)
{
  new_obj_card = card_index;
}

/* Set active issue cards to update */
void GameBoard::setActiveIssueCards(int card_index[5], bool is_new_rotation)
{
  // checkissueSolved();
  if (active_issues.size() >= static_cast<size_t>(game_config.max_issue_cards))
  {
    // Swap To Lose Game State.
    lost_game = true;
  }

  for (int i = 0; i < game_config.max_issue_cards; ++i)
  {
    if (is_new_rotation && active_issue_cards[i] == -1)
    {
      active_issue_cards[i] = card_index[i];
      // sets the slot to active so no other card can take this position.
      // Creating a new issue card and adding it to the back of the
      // current issues vector for rendering and points.
      update_issues = true;
    }
  }
}

/* Set the client's active objective card */
bool GameBoard::updateActiveObjectiveCard()
{
  if (new_obj_card == -1)
  {
    return false;
  }

  delete active_obj_card;
  active_obj_card =
    new ObjectiveCard(static_cast<objective_cards>(new_obj_card));
  debug_text.print("Active objective card set to " +
                   std::to_string(new_obj_card) + ".");
  new_obj_card = -1;

  return true;
}

/* Update the active issue cards if required */
bool GameBoard::updateActiveIssueCards()
{
  if (!update_issues)
  {
    return false;
  }

  for (int i = 0; i < game_config.max_issue_cards; ++i)
  {
    if (active_issue_cards[i] != -1 && !slot_active[i])
    {
      active_issues.emplace_back(
        IssueCard(static_cast<issue_cards>(active_issue_cards[i])));
      slot_active[i] = true;
      debug_text.print("Creating issue card " +
                       std::to_string(active_issue_cards[i]) + ".");
      handleIssueCardEvents(static_cast<issue_cards>(active_issue_cards[i]));
    }
  }
  update_issues = false;

  return true;
}

/* Assign action points to specified issue card */
void GameBoard::assignActionPointToIssue(player_classes _class,
                                         int _issue,
                                         int _points)
{
  active_issues.at(_issue).addActionPoints(_class, _points);
}

/* Render the board */
void GameBoard::render(bool _obj_popup, bool _issue_popup)
{
  // Ship
  m_ship.render();

  // Players
  m_players->render(game_global_scenes::IN_GAME);

  if (_obj_popup)
  {
    // Position card and resize appropriately if it exists (should do)
    if (active_obj_card != nullptr)
    {
      active_obj_card->setDimensions(card_offsets.obj_popup_size);
      active_obj_card->setPosition(card_offsets.obj_popup_pos);
      active_obj_card->render();
    }
  }
  else
  {
    int card_index = 0;
    for (auto& active_issue : active_issues)
    {
      // Position cards and resize appropriately
      if (_issue_popup)
      {
        active_issue.setDimensions(card_offsets.issue_popup_size);
        active_issue.setPosition(
          card_offsets.issue_popup_start +
          (card_offsets.issue_popup_offset * static_cast<float>(card_index)));
      }
      else
      {
        active_issue.setDimensions(card_offsets.issue_ingame_size);
        active_issue.setPosition(
          card_offsets.issue_ingame_start +
          (card_offsets.issue_ingame_offset * static_cast<float>(card_index)));
      }

      // Render
      active_issue.render();
      ++card_index;
    }

    // Render objective card in-game
    if (!_issue_popup && active_obj_card != nullptr)
    {
      active_obj_card->setDimensions(card_offsets.obj_ingame_size);
      active_obj_card->setPosition(card_offsets.obj_ingame_pos);
      active_obj_card->render();
    }
  }

  // Render Item Card in-game
  // if !item card popup.
  int item_card_index = 0;
  for (auto& active_item : item_inventory)
  {
    active_item.setDimensions(card_offsets.item_ingame_size);
    active_item.setPosition(
      card_offsets.item_ingame_start +
      (card_offsets.issue_ingame_offset * static_cast<float>(item_card_index)));
    active_item.render();

    ++item_card_index;
  }

  // Render Obj Inventory
  int obj_card_index = 0;
  for (auto& obj_card : objective_card_inventory)
  {
    obj_card.setDimensions(card_offsets.item_ingame_size);
    obj_card.setPosition(
      card_offsets.obj_ingame_start +
      (card_offsets.obj_ingame_offset * static_cast<float>(obj_card_index)));
    obj_card.render();
    ++obj_card_index;
  }
}

/* Get the room position for a player */
ShipRoom GameBoard::getRoom(ship_rooms _room)
{
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.getEnum() == _room)
    {
      return room;
    }
  }
  throw "Could not find requested room.";
}

/* Get the active issue cards */
std::vector<IssueCard> GameBoard::getIssueCards()
{
  return active_issues;
}

/* Get the count of issue cards */
int GameBoard::activeIssuesCount()
{
  int count = 0;
  for (int i = 0; i < 5; i++)
  {
    if (slot_active[i])
    {
      count++;
    }
  }
  return count;
}

/* Get the active objective card */
ObjectiveCard* GameBoard::getObjectiveCard()
{
  return active_obj_card;
}

/* Handle the events caused by issue cards */
void GameBoard::handleIssueCardEvents(issue_cards _card_type)
{
  // This function will begin the logic of the issue card depending on what
  // issue card it is
  int _card_num = static_cast<int>(_card_type);

  if (_card_num <= 5)
  {
    // Comms issue.
    // Set player state to roll dice and set what kind of event was triggered
    // to do the appropriate action with the dice roll.
    Locator::getPlayers()
      ->getPlayer(
        static_cast<player_classes>(Locator::getPlayers()->my_player_index))
      ->setDiceRolls(1);
  }
  else if (_card_num >= 6 && _card_num <= 11)
  {
    // Engin issue.
    if (_card_num == 8)
    {
      // set countdown on issue card here.
      engine_countdown = 2;
      if (engine_countdown >= 1)
      {
        engine_countdown -= 1;
      }
    }
    else if (_card_num == 10)
    {
      // set item card multiplier to 2.
      for (auto& i : item_inventory)
      {
        i.setActionPointVariable(5);
      }
    }
  }
  else if (_card_num >= 12 && _card_num <= 17)
  {
    // Medic issue.
    // subtract 3 this turn on all issue cards here.
    if (_card_num == 14)
    {
      for (auto& active_issue : active_issues)
      {
        active_issue.setIssueCardvariable(-3);
      }
    }
  }
  else if (_card_num >= 18 && _card_num <= 23)
  {
    // Global issue.
    switch (_card_num)
    {
      case 18:
      {
        // Set max items to 5 for local player.
        Locator::getPlayers()
          ->getPlayer(
            static_cast<player_classes>(Locator::getPlayers()->my_player_index))
          ->setMaxItems(5);
        break;
      }
      case 19:
      {
        // This players items are disabled this turn.
        for (auto& i : item_inventory)
        {
          i.setActive(false);
        }
        break;
      }
      case 20:
      {
        // Chicken on board, player 4 - 1 cant play this turn.
        // Currently only disables the player index 1.
        if (Locator::getPlayers()->my_player_index == 1)
        {
          Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->setChasingChicken(true);
        }
        break;
      }
      case 21:
      {
        // All issues +3 to completion.
        for (auto& active_issue : active_issues)
        {
          active_issue.setIssueCardvariable(3);
        }
        break;
      }
      case 22:
      {
        // All items are overused, value depreciates by 3 this turn.
        for (auto& i : item_inventory)
        {
          i.setActionPointVariable(-3);
        }
        break;
      }
      case 23:
      {
        // Low resources - Discard all items from this player.
        item_inventory.clear();
        break;
      }
      default:
        break;
    }
  }
  else if (_card_num >= 24 && _card_num <= 29)
  {
    // Pilot issue.
    switch (_card_num)
    {
      case 25:
      {
        // If this is a pilot player, roll dice, if less than 4, move back, else
        // move forward.
        if (Locator::getPlayers()->my_player_index ==
            static_cast<int>(player_classes::PILOT))
        {
          pilot_blackhole = true;
          Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->setDiceRolls(1);
        }
        break;
      }
      case 26:
      {
        // If this player is the pilot, roll dice, if above 4, move ship
        // forward.
        if (Locator::getPlayers()->my_player_index !=
            static_cast<int>(player_classes::PILOT))
        {
          bonus_movement = true;
          Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->setDiceRolls(1);
          /* this is just to test if this feature is working, needs to moved to
           * when player rolls dice and issue has been activated. see below.
           *
           * */
          DataShare new_share =
            DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
          new_share.add(Locator::getPlayers()->current_progress_index - 2);
          Locator::getNetworkInterface()->sendData(new_share);
        }
        break;
      }
      default:
        break;
    }
  }
  issueTracking();
}

void GameBoard::setActiveItemCard(int card_index)
{
  for (int i = 0; i < Locator::getPlayers()
                        ->getPlayer(static_cast<player_classes>(
                          Locator::getPlayers()->my_player_index))
                        ->getMaxItems();
       ++i)
  {
    if (active_item_card[i] == -1)
    {
      active_item_card[i] = card_index;
    }
  }
}

bool GameBoard::updateActiveItemCard(int _item_card_index)
{
  if (_item_card_index >= 0 && _item_card_index <= 19)
  {
    item_inventory.emplace_back(static_cast<item_cards>(_item_card_index));
    debug_text.print(
      "Creating item card: " +
      std::to_string(active_item_card[item_inventory.size() - 1]));
    return true;
  }
  return false;
}

void GameBoard::issueTracking()
{
  for (auto& issue : active_issues)
  {
    if (issue.getCardID() == 8)
    {
      engine_countdown -= 1;
      if (engine_countdown == 0)
      {
        // If card has been around for 2 turns, move ship progress back 2
        // spaces.
        auto new_share = DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
        new_share.add(Locator::getPlayers()->current_progress_index - 2);
        Locator::getNetworkInterface()->sendData(new_share);
      }
    }
  }
}

void GameBoard::resetCardVariables()
{
  // Reset Item card values for any issue cards drawn affecting stats.
  for (auto& item : item_inventory)
  {
    item.setActive(true);
    item.setActionPointVariable(0);
  }

  // Reset Issue Cards ".
  for (auto& issue : active_issues)
  {
    issue.setIssueCardvariable(0);
  }
}

void GameBoard::checkissueSolved()
{
  // Remove solved issues
  bool issue_solved = false;
  for (size_t i = 0; i < active_issues.size(); ++i)
  {
    if (active_issues[i].isSolved())
    {
      issue_solved = true;
      active_issues.erase(active_issues.begin() + static_cast<int>(i));
      active_issue_cards[i] = -1;
      slot_active[i] = false;
    }
  }
  // if any cards have been completed and deleted when client ends turn we want
  // to update the server active_issue_cards too.
  if (issue_solved)
  {
    auto new_share = DataShare(data_roles::CLIENT_SOLVED_ISSUE_CARD);
    new_share.add(active_issue_cards[0]);
    new_share.add(active_issue_cards[1]);
    new_share.add(active_issue_cards[2]);
    new_share.add(active_issue_cards[3]);
    new_share.add(active_issue_cards[4]);
    Locator::getNetworkInterface()->sendData(new_share);
  }
}

void GameBoard::syncIssueCards(int active_cards[5])
{
  for (int i = 0; i < 5; ++i)
  {
    active_issue_cards[i] = active_cards[i];
  }
}

bool GameBoard::checkObjectiveCardComplete(player_classes _this_clients_class)
{
  for (auto& issue : active_issues)
  {
    if (active_obj_card->objectiveComplete(&issue, _this_clients_class))
    {
      return true;
    }
  }
  return false;
}

void GameBoard::addObjCardToInventory()
{
  objective_card_inventory.emplace_back(active_obj_card->getCardID());
}

void GameBoard::useObjCardDebug()
{
  if (!objective_card_inventory.empty())
  {
    objective_card_inventory.front().useObjectiveCard();
  }
}
