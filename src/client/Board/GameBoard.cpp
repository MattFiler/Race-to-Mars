#include "GameBoard.h"

bool GameBoard::checkForClicks(Vector2 clicked_pos)
{
  for (ShipRoom& room : m_ship.getRooms())
  {
    if (room.isInBoundingBox(clicked_pos))
    {
      debug_text.print("Clicked in room: " + room.getName());
      return true;
    }
  }
  return false;
}

void GameBoard::render()
{
  m_ship.render();
  m_players.render();
}