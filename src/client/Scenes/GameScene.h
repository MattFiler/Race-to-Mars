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

  void handleIssueCardEvents(issue_cards _card_type);

 private:
  GameBoard board;
  Menu pause_menu;

  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };

  SceneUI ui_manager;

  bool is_new_turn = false;
  bool got_new_obj_card = false;
  bool got_new_obj_this_turn = false;

  CardOffsets card_offsets;

  int max_progress_index = 19; // win condition
  double popup_timer = 0.0f;

  bool has_disconnected = false; // did local client disconnect?
  game_state current_state = game_state::PLAYING;
  // int current_progress_index = 0;
  bool current_scene_lock_active = false; // optional "scene lock" to freeze
                                          // client interaction - useful for the
                                          // end of a turn?
};
#endif // PROJECT_GAMESCENE_H
