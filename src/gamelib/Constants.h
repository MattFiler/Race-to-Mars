#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

#include "gamelib/Math/Vector2.h"

/* Scenes */
enum game_global_scenes
{
  DEFAULT = -1,
  MAIN_MENU = 0,
  TUTORIAL,
  LOBBY,
  IN_GAME,
  QUIT_GAME
};

/* In-game states */
enum game_state
{
  PLAYING,     // The regular in-game state
  LOCAL_PAUSE, // The quit menu
};

/* Misc config */
struct GameConfig
{
  int max_issue_cards = 5;
};

/* Won or lost? */
enum win_state
{
  UNDECIDED,
  LOST,
  WON
};

/* In-Game UI Elements */
enum ui_sprites
{
  BACKGROUND,
  ACTIVE_PLAYER_MARKER,
  YOUR_PLAYER_MARKER,
  INACTIVE_PLAYER_MARKER,
  PROGRESS_METER,
  PROGRESS_MARKER,
  SYNC_OVERLAY,
  DISCONNECT_OVERLAY,
  OTHER_PLAYER_DISCONNECTED_OVERLAY,
  CHAT_BOX,
  MSG_ALERT,
  POPUP_CARD_SHADOWS_0,
  POPUP_CARD_SHADOWS_1,
  POPUP_CARD_SHADOWS_2,
  POPUP_CARD_SHADOWS_3,
  POPUP_CARD_SHADOWS_4,
  POPUP_CARD_SHADOWS_5,
  DICE_ROLL_1,
  DICE_ROLL_2,
  DICE_ROLL_3,
  DICE_ROLL_4,
  DICE_ROLL_5,
  DICE_ROLL_6
};
enum ui_popups
{
  ISSUE_POPUP,
  OBJECTIVE_POPUP,
  ITEM_POPUP,
  DICE_ROLL_POPUP,
  CHICKEN_POPUP,
  YOU_WIN_POPUP,
  YOU_LOSE_POPUP
};
enum ui_buttons
{
  END_TURN_BTN,
  BUY_ITEM_BTN,
  ROLL_DICE_BTN,
  CHAT_BTN,
  OBJECTIVE_BTN
};

/* Tutorial elements */
enum ui_tutorial
{
  NUMBER_OF_SLIDES = 13
};
enum ui_tutorial_btns
{
  CONTINUE_BUTTON,
  BACK_BUTTON
};

/* UI card offsets */
struct CardOffsets
{
  // Issue card popup
  Vector2 issue_popup_start = Vector2(86, 205); // Position of card 1
  Vector2 issue_popup_offset = Vector2(222, 0); // Offset from 1
  Vector2 issue_popup_size = Vector2(222, 310); // Size of card

  // Issue card popup buttons
  Vector2 issue_popup_ap_btn_start = Vector2(106, 543); // Position of button 1
  Vector2 issue_popup_ap_btn_offset = Vector2(221, 0);  // Offset of each button

  // Issue card in-game
  Vector2 issue_ingame_start = Vector2(1100, 20); // Position of card 1
  Vector2 issue_ingame_offset = Vector2(0, 30);   // Offset from 1
  Vector2 issue_ingame_size = Vector2(124, 175);  // Size of card

  // Objective card popup
  Vector2 obj_popup_pos = Vector2(132, 141);  // Position of card (only 1)
  Vector2 obj_popup_size = Vector2(304, 424); // Size of card

  // Objective card in-game
  Vector2 obj_ingame_pos = Vector2(486, 70);   // Position of card (only 1)
  Vector2 obj_ingame_size = Vector2(124, 175); // Size of card

  // obj card in-game inventory
  Vector2 obj_ingame_start = Vector2(420, 540); // Position of card 1
  Vector2 obj_ingame_offset = Vector2(30, 0);   // Offset from 1

  // Item card popup
  Vector2 item_popup_start = Vector2(86, 205); // Position of card 1 / 2 - 5
  Vector2 item_popup_offset = Vector2(222, 0); // Offset from 1
  Vector2 item_popup_size = Vector2(222, 310); // Size of card

  // Item card popup buttons
  Vector2 item_popup_ap_btn_start = Vector2(106, 543); // Position of button 1
  Vector2 item_popup_ap_btn_offset = Vector2(145, 0);  // Offset of each button

  // Item card in-game
  Vector2 item_ingame_start = Vector2(1100, 330); // Position of card 1 / 1 - 5
  Vector2 item_ingame_offset = Vector2(0, 30);    // Offset from 1
  Vector2 item_ingame_size = Vector2(124, 175);   // Size of card

  // Misc
  Vector2 card_shadow_offset = Vector2(80, 80);   // Single-card shadow
  Vector2 ap_btn_shadow_offset = Vector2(25, 25); // AP button shadow
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
