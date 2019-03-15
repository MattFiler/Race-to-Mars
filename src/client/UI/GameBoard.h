#ifndef PROJECT_GAMEBOARD_H
#define PROJECT_GAMEBOARD_H

#include "client/Players/AllPlayers.h"
#include "client/UI/Ship.h"
#include <Engine/Renderer.h>

/* Ideally this class will know all about stuff on the board - E.G. active
 * cards, active counters, active player tokens, etc - then all rendering can be
 * done by calling this class' render function. */

class GameBoard
{
 public:
  GameBoard() = default;
  ~GameBoard() = default;

  void render();

 private:
  Players m_players;
  Ship m_ship;
};

#endif // PROJECT_GAMEBOARD_H
