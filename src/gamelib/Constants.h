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
  PLAYING,              // The regular in-game state
  LOCAL_PAUSE,          // The quit menu
  ISSUE_CARDS_POPUP,    // New round, show issue cards
  OBJECTIVE_CARD_POPUP, // New objective card added
  IS_ROLLING_DICE       // Active player rolls dice
};

enum render_order
{
  PRIORITY_BACKGROUND,
  PRIORITY_MIDDLE,
  PRIORITY_UI,
  PRIORITY_TEXT,
  PRIORITY_UI_2,
  PRIORITY_UI_3,
  PRIORITY_CARD_1_SHADOW, // these are used dynamically
  PRIORITY_CARD_1,
  PRIORITY_CARD_2_SHADOW, // these are used dynamically
  PRIORITY_CARD_2,        // these are used dynamically
  PRIORITY_CARD_3_SHADOW, // these are used dynamically
  PRIORITY_CARD_3,        // these are used dynamically
  PRIORITY_CARD_4_SHADOW, // these are used dynamically
  PRIORITY_CARD_4,        // these are used dynamically
  PRIORITY_CARD_5_SHADOW, // these are used dynamically
  PRIORITY_CARD_5,        // these are used dynamically
  PRIORITY_CARD_6_SHADOW, // these are used dynamically
  PRIORITY_CARD_6,
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
  // Issue card popup
  Vector2 issue_popup_start = Vector2(86, 205); // Position of card 1
  Vector2 issue_popup_offset = Vector2(222, 0); // Offset from 1
  Vector2 issue_popup_size = Vector2(222, 310); // Size of card

  // Issue card in-game
  Vector2 issue_ingame_start = Vector2(1100, 70); // Position of card 1
  Vector2 issue_ingame_offset = Vector2(0, 75);   // Offset from 1
  Vector2 issue_ingame_size = Vector2(124, 175);  // Size of card

  // Objective card popup
  Vector2 obj_popup_pos = Vector2(132, 141);  // Position of card (only 1)
  Vector2 obj_popup_size = Vector2(304, 424); // Size of card

  // Objective card in-game
  Vector2 obj_ingame_pos = Vector2(486, 70);   // Position of card (only 1)
  Vector2 obj_ingame_size = Vector2(124, 175); // Size of card

  // Misc
  Vector2 shadow_offset = Vector2(80, 80); // Single-card shadow
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
