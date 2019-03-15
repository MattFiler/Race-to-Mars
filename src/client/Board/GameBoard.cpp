#include "GameBoard.h"

/* Check to see if we're HOVERING over an interactable item  */
bool GameBoard::isHoveringOverInteractable(Vector2 hover_pos)
{
  return cursorPosFallsIntoClickable(hover_pos);
}

/* Check to see if we've CLICKED on an interactable item  */
bool GameBoard::didClickOnInteractable(Vector2 clicked_pos)
{
  return cursorPosFallsIntoClickable(clicked_pos);
}

/* Generic check to see if vector position falls within any bounding box */
bool GameBoard::cursorPosFallsIntoClickable(Vector2 pos)
{
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(pos))
    {
      return true;
    }
  }
  return false;
}

/* Render the board */
void GameBoard::render()
{
  m_ship.render();
  m_players.render();
}