#include "GameBoard.h"
#include "gamelib/Constants.h"

/* Check to see if we're HOVERING over an interactable item  */
bool GameBoard::isHoveringOverInteractable(Vector2 hover_pos)
{
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(hover_pos))
    {
      return true;
    }
  }

  //  if(m_item_deck.isInBoundingBox(hover_pos))
  //  {
  //    return true;
  //  }
  return false;
}

/* Return CLICKED interactable item (check for click first)  */
ShipRoom GameBoard::getClickedInteractable(Vector2 clicked_pos)
{
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(clicked_pos))
    {
      return room;
    }
  }
  throw std::runtime_error("Tried to access clicked item, but none were found. "
                           "Call isHoveringOverInteractable first to "
                           "validate!");
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
  }

  for (int i = 0; i < game_config.max_issue_cards; ++i)
  {
    if (is_new_rotation)
    {
      // if current turn is % 3 then set new obj card to true.
      // check to see if any cards changed during turn.
      if (active_issue_cards[i] != card_index[i])
      {
        active_issue_cards[i] = card_index[i];
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
      if (active_issue_cards[i] != -1)
      {
        active_issues.emplace_back(
          IssueCard(static_cast<issue_cards>(active_issue_cards[i])));
        active_issues[i].getSprite()->setPos(
          Vector2(static_cast<float>(i) * 257, 150.0f));

        debug_text.print("Creating issue card " +
                         std::to_string(active_issue_cards[i]) + ".");
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
        break;
      }
      case 26:
      {
        // If this player is the pilot, roll dice, if above 4, move ship
        // forward.
        break;
      }
      default:
        break;
    }
  }
}
