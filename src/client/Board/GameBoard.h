#ifndef PROJECT_GAMEBOARD_H
#define PROJECT_GAMEBOARD_H

#include "client/Board/Ship.h"
#include "client/Players/AllPlayers.h"
#include "gamelib/Debug/DebugText.h"
#include <Engine/Renderer.h>

/* Ideally this class will know all about stuff on the board - E.G. active
 * cards, active counters, active player tokens, etc - then all rendering can be
 * done by calling this class' render function. */

class GameBoard
{
 public:
  GameBoard() = default;
  ~GameBoard() = default;

  bool isHoveringOverInteractable(Vector2 hover_pos);
  bool didClickOnInteractable(Vector2 clicked_pos);

  void render();

 private:
  bool cursorPosFallsIntoClickable(Vector2 pos);
  Players m_players;
  Ship m_ship;

  DebugText debug_text;
};

#endif // PROJECT_GAMEBOARD_H
