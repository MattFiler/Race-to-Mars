#ifndef PROJECT_GAMESCENE_H
#define PROJECT_GAMESCENE_H

#include "Scene.h"
#include "client/Board/GameBoard.h"
#include "client/UI/Cursor.h"
#include "client/UI/Managers/GameSceneUI.h"
#include "client/UI/Managers/PopupManager.h"
#include "client/UI/PopupWindow.h"
#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>

/* UI Elements */
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
  DICE_ROLL_POPUP
};
enum ui_buttons
{
  END_TURN_BTN,
  BUY_ITEM_BTN,
  ROLL_DICE_BTN
};

class GameScene : public Scene
{
 public:
  GameScene();
  ~GameScene() override;

  void init() override;

  void networkConnected() override;
  void networkDisconnected() override;
  void networkDataReceived(const enet_uint8* data, size_t data_size) override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;
  LobbyPlayer* getLobbyPlayer(int const _player_index)
  {
    return players[_player_index];
  }

 private:
  void debugOutput();

  GameBoard board;

  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };

  SceneUI ui_manager;

  bool is_new_turn = false;
  bool got_new_obj_card = false;
  bool got_new_obj_this_turn = false;
  bool rolled_dice_this_turn = false;
  bool free_player_movement = false;

  bool update_item_card = false;
  int new_item_card = -1;

  CardOffsets card_offsets;

  int max_progress_index = 19; // win condition

  bool has_disconnected = false; // did local client disconnect?
  game_state current_state = game_state::PLAYING;
  // int current_progress_index = 0;
  bool current_scene_lock_active = false; // optional "scene lock" to freeze
                                          // client interaction - useful for the
                                          // end of a turn?
};
#endif // PROJECT_GAMESCENE_H
