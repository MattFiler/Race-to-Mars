#include "GameBoard.h"

void GameBoard::render()
{
  m_ship.render();
  m_players.render();
}