#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

#include "gamelib/Math/Vector2.h"

enum game_global_scenes
{
  DEFAULT = -1,
  MAIN_MENU = 0,
  LOBBY,
  IN_GAME,
  QUIT_GAME
};

enum game_state
{
  PLAYING,         // The regular in-game state
  LOCAL_PAUSE,     // The quit menu
  NEW_CARDS_POPUP, // The player info popup
  // ROLL_DICE
};

enum render_order
{
  PRIORITY_BACKGROUND,
  PRIORITY_MIDDLE,
  PRIORITY_UI,
  PRIORITY_TEXT,
  PRIORITY_UI_2,
  PRIORITY_CARDS,
  PRIORITY_TOPMOST,
  PRIORITY_OVERLAYS,
  PRIORITY_CURSOR
};

struct GameConfig
{
  int max_issue_cards = 5;
};

struct CardOffsets
{
  // Popup
  Vector2 popup_start = Vector2(86, 205); // Position of card 1
  Vector2 popup_offset = Vector2(222, 0); // Offset from 1
  Vector2 popup_size = Vector2(222, 310); // Size of card

  // In-game
  Vector2 ingame_start = Vector2(1100, 70); // Position of card 1
  Vector2 ingame_offset = Vector2(0, 75);   // Offset from 1
  Vector2 ingame_size = Vector2(124, 175);  // Size of card
};

// Resolution (set at runtime)
struct GameResolution
{
 public:
  static int height;
  static int width;
  static float scale;
  static const int base_height = 720;
  static const int base_width = 1280;
};

#endif // PROJECT_CONSTANTS_H
