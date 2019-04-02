#include "GameBoard.h"
#include "gamelib/Constants.h"

/* Check to see if we're hovering over an interactable item  */
hovered_type GameBoard::isHoveringOverInteractable(Vector2 hover_pos)
{
  // Check ship rooms
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(hover_pos))
    {
      return hovered_type::HOVERED_OVER_SHIP_ROOM;
    }
  }

  // Check issue cards
  for (IssueCard& card : active_issues)
  {
    if (card.isInBoundingBox(hover_pos))
    {
      return hovered_type::HOVERED_OVER_ISSUE_CARD;
    }
  }

  // Check objective card
  if (active_obj_card != nullptr && active_obj_card->isInBoundingBox(hover_pos))
  {
    return hovered_type::HOVERED_OVER_OBJECTIVE_CARD;
  }

  return hovered_type::DID_NOT_HOVER_OVER_ANYTHING;
}

/* Return clicked interactable room  */
ShipRoom GameBoard::getClickedInteractableRoom(Vector2 clicked_pos)
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
        "isHoveringOverInteractable.";
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
        "isHoveringOverInteractable.";
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
        "isHoveringOverInteractable.";
}

/* Set the active objective card to update */
void GameBoard::setActiveObjectiveCard(int card_index)
{
  new_obj_card = card_index;
}

/* Set active issue cards to update */
void GameBoard::setActiveIssueCards(int card_index[5], bool is_new_rotation)
{
  for (size_t i = 0; i < active_issues.size(); ++i)
  {
    if (active_issues[i].isSolved())
    {
      active_issues.erase(active_issues.begin() + static_cast<int>(i));
      active_issue_cards[i] = -1;
    }
  }

  if (active_issues.size() >= static_cast<size_t>(game_config.max_issue_cards))
  {
    // Swap To Lose Game State.
    lost_game = true;
  }

  for (int i = 0; i < game_config.max_issue_cards; ++i)
  {
    if (is_new_rotation)
    {
      // check to see if any cards changed during turn.
      if (active_issue_cards[i] != card_index[i])
      {
        active_issue_cards[i] = card_index[i];

        // sets the slot to active so no other card can take this position.
        // Creating a new issue card and adding it to the back of the
        // current issues vector for rendering and points.
        update_issues = true;
      }
    }
  }
}

/* Set the client's active objective card */
void GameBoard::updateActiveObjectiveCard()
{
  if (new_obj_card != -1)
  {
    if (active_obj_card != nullptr)
    {
      delete active_obj_card;
    }
    active_obj_card =
      new ObjectiveCard(static_cast<objective_cards>(new_obj_card));
    debug_text.print("Active objective card set to " +
                     std::to_string(new_obj_card) + ".");
    new_obj_card = -1;
  }
}

/* Update the active issue cards if required */
void GameBoard::updateActiveIssueCards()
{
  if (update_issues)
  {
    for (int i = 0; i < game_config.max_issue_cards; ++i)
    {
      if (active_issue_cards[i] != -1 && slot_active[i] == false)
      {
        active_issues.emplace_back(
          IssueCard(static_cast<issue_cards>(active_issue_cards[i])));
        active_issues[i].getSprite()->setPos(
          Vector2(static_cast<float>(i) * 257, 150.0f));
        slot_active[i] = true;
        debug_text.print("Creating issue card " +
                         std::to_string(active_issue_cards[i]) + ".");
        handleIssueCardEvents(static_cast<issue_cards>(active_issue_cards[i]));
      }
    }
    update_issues = false;
  }
}

/* Render the board */
void GameBoard::render()
{
  // Ship
  m_ship.render();

  // Players
  m_players->render(game_global_scenes::IN_GAME);

  // Issue cards
  for (auto& active_issue : active_issues)
  {
    active_issue.render();
  }

  // Objective card
  if (active_obj_card != nullptr)
  {
    active_obj_card->render();
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

/* Handle the events caused by issue cards */
void GameBoard::handleIssueCardEvents(issue_cards _card_type)
{
  // This function will begin the logic of the issue card depending on what
  // issue card it is
  int _card_num = static_cast<int>(_card_type);

  if (_card_num <= 5)
  {
    // Comms issue.
    // Call player roll dice function here.
    Locator::getPlayers()
      ->getPlayer(
        static_cast<player_classes>(Locator::getPlayers()->my_player_index))
      ->setDiceRolls(1);
  }
  else if (_card_num >= 6 && _card_num <= 11)
  {
    // Engin issue.
    switch (_card_num)
    {
      case 6:
      {
        break;
      }
      case 7:
      {
        break;
      }
      case 8:
      {
        break;
      }
      case 10:
      {
        // set item card multiplier to 2.
        for (auto& i : item_inventory)
        {
          i.setActionPointVariable(5);
        }
      }
      default:
        break;
    }
  }
  else if (_card_num >= 12 && _card_num <= 17)
  {
    // Medic issue.
    // subtract 3 this turn on all issue cards here.
    for (auto& active_issue : active_issues)
    {
      active_issue.setIssueCardvariable(-3);
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
          Locator::getPlayers()->getPlayer(static_cast<player_classes>(
            Locator::getPlayers()->my_player_index));
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
        // Low rsources - Discard all items from this player.
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
          pilot_blckhole = true;
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
        if (Locator::getPlayers()->my_player_index ==
            static_cast<int>(player_classes::PILOT))
        {
          pilot_moveforward = true;
          Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->setDiceRolls(1);
        }
        break;
      }
      default:
        break;
    }
  }
}
