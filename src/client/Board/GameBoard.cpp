#include "GameBoard.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "gamelib/Constants.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include <chrono>
#include <client/Cards/ObjectiveCard.h>
#include <thread>

/*
 *
 * This class features functions intended for use in different threads - each
 * function is commented with its intended use. The general rule of thumb is if
 * the function uses a vector of cards, it MUST be used in the game scene.
 *
 * We have arrays of card indexes which are safe to use in the network thread -
 * due to sprite issues when accessing card vectors in the network thread, this
 * must be stuck to!
 *
 * It also means we need to be careful to update our vector when new cards are
 * added to the index array from the network thread. It should be checked each
 * game update tick.
 *
 */

/* Check to see if we're hovering over an interactable room  */
/* THREAD: GAME */
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

/* Check to see if we're hovering over an item card  */
/* THREAD: GAME */
bool GameBoard::isHoveringOverItemCard(Vector2 hover_pos)
{
  for (ItemCard& card : item_inventory)
  {
    if (card.isInBoundingBox(hover_pos))
    {
      return true;
    }
  }
  return false;
}

/* Check to see if we're hovering over an issue card  */
/* THREAD: GAME */
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
/* THREAD: GAME */
bool GameBoard::isHoveringOverObjectiveCard(Vector2 hover_pos)
{
  return active_obj_card != nullptr &&
         active_obj_card->isInBoundingBox(hover_pos);
}

/* Return clicked interactable room  */
/* THREAD: GAME */
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
/* THREAD: GAME */
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
/* THREAD: GAME */
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
/* THREAD: NETWORK */
void GameBoard::setActiveObjectiveCard(int card_index)
{
  new_obj_card = card_index;
}

/* Set active issue cards to update */
/* THREAD: NETWORK */
void GameBoard::setActiveIssueCards(int card_index[5], bool is_new_rotation)
{
  for (int i = 0; i < game_config.max_issue_cards; ++i)
  {
    if (is_new_rotation && active_issue_cards[i] == -1)
    {
      active_issue_cards[i] = card_index[i];
      slot_active[i] = false;
      debug_text.print("@setActiveIssueCards - Active issue card at " +
                       std::to_string(i) + " is set to id " +
                       std::to_string(card_index[i]) + ".");
      // sets the slot to active so no other card can take this position.
      // Creating a new issue card and adding it to the back of the
      // current issues vector for rendering and points.
      update_issues = true;
    }
    else if (active_issue_cards[i] != card_index[i])
    {
      debug_text.print("@setActiveIssueCards - Our local card info was wrong "
                       "(" +
                       std::to_string(i) + " - overwriting...");
      active_issue_cards[i] = card_index[i];
      slot_active[i] = false;
    }
    else
    {
      debug_text.print("@setActiveIssueCards - Card at index " +
                       std::to_string(i) + " already set, continuing...");
    }
  }
  debug_text.print("@setActiveIssueCards - Finished setting active issue card "
                   "array");

  // Debug out current card array
  for (int i = 0; i < 5; i++)
  {
    debug_text.print("@setActiveIssueCards - Card " + std::to_string(i) +
                     " in array is ID " +
                     std::to_string(active_issue_cards[i]));
  }
}

/* Set the client's active objective card */
/* THREAD: GAME */
bool GameBoard::updateActiveObjectiveCard()
{
  if (new_obj_card == -1)
  {
    return false;
  }

  delete active_obj_card;
  active_obj_card =
    new ObjectiveCard(static_cast<objective_cards>(new_obj_card));
  debug_text.print("@updateActiveObjectiveCard - Active objective card set "
                   "to " +
                   std::to_string(new_obj_card) + ".");
  new_obj_card = -1;

  return true;
}

/* Update the active issue cards if required */
/* THREAD: GAME */
bool GameBoard::updateActiveIssueCards()
{
  if (!update_issues)
  {
    return false;
  }

  // Quickly hard lock here if cards are being updated - avoids thread errors
  while (is_updating_cards)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
  }

  // Check to see if we've solved any issues...
  checkIssueSolved();

  // Update our issue card vector (used by the game thread)
  for (int i = 0; i < game_config.max_issue_cards; ++i)
  {
    if (active_issue_cards[i] != -1 && !slot_active[i])
    {
      active_issues.emplace_back(
        IssueCard(static_cast<issue_cards>(active_issue_cards[i].load())));
      slot_active[i] = true;
      debug_text.print("@updateActiveIssueCards - Creating issue card " +
                       std::to_string(active_issue_cards[i]) +
                       " in vector at " +
                       std::to_string(active_issues.size() - 1) + ".");
      handleIssueCardEvents(
        static_cast<issue_cards>(active_issue_cards[i].load()));
    }
  }
  update_issues = false;

  return true;
}

/* Assign action points to specified issue card */
/* THREAD: GAME */
bool GameBoard::assignActionPointToIssue(player_classes _class,
                                         int _issue,
                                         int _points)
{
  if (_issue >= static_cast<int>(active_issues.size()))
  {
    // Should never hit this!
    debug_text.print("@assignActionPointToIssue - Hmm... strange! A player "
                     "assigned action points to a "
                     "card we don't know about.");
    return false; // We got data for a card we don't know about... it's resync
                  // time!
  }
  if (!active_issues.at(static_cast<size_t>(_issue)).isSolved())
  {
    // Assign action points by class
    active_issues.at(static_cast<size_t>(_issue))
      .addActionPoints(_class, _points);
  }
  return true;
}

/* Render the board */
/* THREAD: GAME */
void GameBoard::render(bool _obj_popup, bool _issue_popup, bool _item_popup)
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
    // Position cards and resize appropriately
    if (_item_popup)
    {
      active_item.setDimensions(card_offsets.item_popup_size);
      active_item.setPosition(
        card_offsets.item_popup_start +
        (card_offsets.item_popup_offset * static_cast<float>(item_card_index)));
    }
    else
    {
      active_item.setDimensions(card_offsets.item_ingame_size);
      active_item.setPosition(card_offsets.item_ingame_start +
                              (card_offsets.item_ingame_offset *
                               static_cast<float>(item_card_index)));
    }

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
/* THREAD: GAME */
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
/* THREAD: GAME */
std::vector<IssueCard> GameBoard::getIssueCards()
{
  return active_issues;
}

/* Get the item cards */
/* THREAD: GAME */
std::vector<ItemCard> GameBoard::getItemCards()
{
  return item_inventory;
}

/* Get the count of issue cards */
/* THREAD: GAME */
int GameBoard::activeIssuesCount()
{
  debug_text.print("@activeIssuesCount - requested issue count, it is " +
                   std::to_string(active_issues.size()));
  return static_cast<int>(active_issues.size());
}

/* Get the count of item cards */
/* THREAD: GAME */
int GameBoard::itemCardCount()
{
  return static_cast<int>(item_inventory.size());
}

/* Get the active objective card */
/* THREAD: GAME */
ObjectiveCard* GameBoard::getObjectiveCard()
{
  return active_obj_card;
}

/* Handle the events caused by issue cards */
/* THREAD: GAME */
void GameBoard::handleIssueCardEvents(issue_cards _card_type)
{
  // This function will begin the logic of the issue card depending on what
  // issue card it is

  switch (_card_type)
  {
    case issue_cards::COMMS__GOOD_COMMUNICATION:
    {
      // Set player state to roll dice and set what kind of event was triggered
      // to do the appropriate action with the dice roll.
      good_communication = true;
      break;
    }
    case issue_cards ::ENGIN__ENGINE_MALFUNCTION:
    {
      // set countdown on issue card here.
      engine_countdown = 2;
      if (engine_countdown >= 1)
      {
        engine_countdown -= 1;
      }
      break;
    }
    case issue_cards::ENGIN__ITEM_BOOST:
    {
      // set item card multiplier to 2.
      for (auto& i : item_inventory)
      {
        i.setActionPointVariable(5);
      }
      break;
    }
    case issue_cards ::MEDIC__HEALTHY_CREW:
    {
      // Medic issue.
      // subtract 3 this turn on all issue cards here.
      for (auto& active_issue : active_issues)
      {
        active_issue.setIssueCardVariable(-3);
      }
      break;
    }
    case issue_cards ::GLOBL__LOW_RESOURCES:
    {
      // Low resources - Discard all items from this player.
      item_inventory.clear();
      break;
    }
    case issue_cards ::GLOBL__ITEMS_OVERUSED:
    {
      // All items are overused, value depreciates by 3 this turn.
      for (auto& i : item_inventory)
      {
        i.setActionPointVariable(-3);
      }
      break;
    }
    case issue_cards ::GLOBL__GRAVITY_LEAK:
    {
      // All issues +3 to completion.
      for (auto& active_issue : active_issues)
      {
        active_issue.setIssueCardVariable(3);
      }
      break;
    }
    case issue_cards ::GLOBL__CHICKEN_ON_BOARD:
    {
      // Chicken on board, player 4 - 1 cant play this turn.
      // Currently only disables the player index 1.
      if (Locator::getPlayers()->my_player_index == 1)
      {
        // call server function that generates random number, show a popup to
        // that player to tell them they are chasing chicken this turn.
        Locator::getPlayers()
          ->getPlayer(
            static_cast<player_classes>(Locator::getPlayers()->my_player_index))
          ->setChasingChicken(true);
      }
      break;
    }
    case issue_cards ::GLOBL__BROKEN_ITEMS:
    {
      // This players items are disabled this turn.
      for (auto& i : item_inventory)
      {
        i.setActive(false);
      }
      break;
    }
    case issue_cards ::GLOBL__BACKPACK_EXTENDED:
    {
      // Set max items to 5 for local player.
      Locator::getPlayers()
        ->getPlayer(Locator::getPlayers()
                      ->players[Locator::getPlayers()->my_player_index]
                      .current_class)
        ->setMaxItems(5);
      break;
    }
    case issue_cards ::PILOT__BLACK_HOLE:
    {
      // If this is a pilot player, roll dice, if less than 4, move back, else
      // move forward.
      if (Locator::getPlayers()
            ->players[Locator::getPlayers()->my_player_index]
            .current_class == player_classes::PILOT)
      {
        pilot_blackhole = true;
      }
      break;
    }
    case issue_cards ::PILOT__BONUS_MOVEMENT:
    {
      // Roll dice, move ship forward half the amount rolled, not below 1.
      if (Locator::getPlayers()
            ->players[Locator::getPlayers()->my_player_index]
            .current_class == player_classes::PILOT)
      {
        bonus_movement = true;
      }
      break;
    }
    default:
    {
      debug_text.print("@handleIssueCardEvents - Card " +
                       std::to_string(static_cast<int>(_card_type)) +
                       " has no events.");
      break;
    }
  }
  issueTracking();
}

/* Set our active item card (I think we're gonna depreciate this) */
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

/* Update our active item card */
/* THREAD: GAME */
bool GameBoard::updateActiveItemCard(int _item_card_index)
{
  if (_item_card_index >= 0 && _item_card_index <= 19)
  {
    item_inventory.emplace_back(static_cast<item_cards>(_item_card_index));
    debug_text.print(
      "@updateActiveItemCard - Creating item card: " +
      std::to_string(active_item_card[item_inventory.size() - 1]));
    return true;
  }
  return false;
}

/* Keep track of issue abilities */
/* THREAD: GAME */
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

/* Reset our card's variable data, this can cancel out special card modifiers */
/* THREAD: GAME */
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
    issue.setIssueCardVariable(0);
  }
}

/* To call at the end full rotation - checks to see if any cards were solved,
 * and if so, removes them from our vector */
/* THREAD: GAME */
void GameBoard::checkIssueSolved()
{
  // Remove solved issues
  int card_index = 0;
  std::vector<int> cards_solved;
  for (IssueCard& card : active_issues)
  {
    if (card.isSolved())
    {
      debug_text.print("@checkIssueSolved - card ID " +
                       std::to_string(card.getCardID()) +
                       " was marked as SOLVED");
      cards_solved.push_back(card_index);
    }
    else
    {
      debug_text.print("@checkIssueSolved - card ID " +
                       std::to_string(card.getCardID()) + " was NOT SOLVED");
      card_index++; // Only increment if not being deleted, else our index will
                    // be off
    }
  }
  debug_text.print("@checkIssueSolved ---");
  for (int card : cards_solved)
  {
    debug_text.print("@checkIssueSolved - deleting solved card");
    active_issues.erase(active_issues.begin() + card);
  }
}

/* To call each turn - checks to see if any cards were solved this turn, and
 * removes them from our network-safe array */
/* THREAD: GAME */
void GameBoard::checkIssueSolvedThisTurn()
{
  is_updating_cards = true;

  // Remove solved issues from array
  auto new_share = DataShare(data_roles::CLIENT_TURN_SOLVED_ISSUE);
  for (IssueCard& card : active_issues)
  {
    if (card.isSolved())
    {
      bool marked_solved = false;
      for (int i = 0; i < 5; i++)
      {
        if (active_issue_cards[i] == card.getCardID())
        {
          active_issue_cards[i] = -1;
          slot_active[i] = false;
          new_share.add(i);
          marked_solved = true;
          break;
        }
      }
      if (!marked_solved)
      {
        debug_text.print("@checkIssueSolvedThisTurn - FAILED TO FIND CARD ID " +
                         std::to_string(card.getCardID()) +
                         " IN ACTIVE ISSUE CARD ARRAY!!!!");
      }
    }
  }

  // If any cards were solved, let the server know
  if (new_share.retrieve(0) != -1)
  {
    Locator::getNetworkInterface()->sendData(new_share);
  }

  is_updating_cards = false;
}

/* Get the number of issues that are left to solve in our network-safe array */
int GameBoard::getInternalIssuesUnsolved()
{
  int count = 0;
  for (int i = 0; i < 5; i++)
  {
    if (active_issue_cards[i] != -1)
    {
      count++;
    }
  }
  return count;
}

/* Check for completion of objective card */
/* THREAD: GAME */
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

/* Add an objective card to our inventory */
/* THREAD: GAME */
void GameBoard::addObjCardToInventory()
{
  objective_card_inventory.emplace_back(active_obj_card->getCardID());
}

/* Use a completed objective card's ability */
/* THREAD: GAME */
void GameBoard::useObjectiveCard()
{
  if (!objective_card_inventory.empty())
  {
    objective_card_inventory.back().useObjectiveCard();
    objective_card_inventory.pop_back();
  }
}

/* Erase an item card - might be nice to give players this control? */
/* THREAD: GAME */
void GameBoard::eraseItemCard(int _item_card_to_delete)
{
  item_inventory.erase(item_inventory.begin() + _item_card_to_delete);
}

/* Prep for a full card re-sync (depreciate this?) */
/* THREAD: GAME */
void GameBoard::prepReSync()
{
  active_issues.clear();
  for (int i = 0; i < 5; ++i)
  {
    active_issue_cards[i] = -1;
    slot_active[i] = false;
  }
  debug_text.print("@prepReSync - cleared out objective cards");
}
