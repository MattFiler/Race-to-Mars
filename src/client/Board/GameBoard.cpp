#include "GameBoard.h"

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

/* Render the board */
void GameBoard::render()
{
  m_ship.render();
  m_players.render(game_global_scenes::IN_GAME);
}