#ifndef PROJECT_GAMESCENE_H
#define PROJECT_GAMESCENE_H

#include "Scene.h"
#include "client/Board/GameBoard.h"
#include "client/UI/Cursor.h"
#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>

// All sprites for the game ui
struct GameSprites
{
  ScaledSprite* background = nullptr;
  ScaledSprite* inactive_player_marker = nullptr;
  ScaledSprite* active_player_marker = nullptr;
  ScaledSprite* progress_meter = nullptr;
  ScaledSprite* progress_marker = nullptr;
  ScaledSprite* sync_overlay = nullptr;
  ScaledSprite* disconnect_overlay = nullptr;
  ScaledSprite* issue_popup = nullptr;
  ScaledSprite* objective_popup = nullptr;
};

class GameScene : public Scene
{
 public:
  GameScene() { debug_text.print("entered game scene."); };
  ~GameScene() = default;

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

  GameSprites game_sprites;
  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };

  // Clicked cards from the popup - this all needs to be moved to its own class
  // as it doesn't belong here. Will be done!
  ObjectiveCard* clicked_obj_card = nullptr;
  IssueCard* clicked_issue_card = nullptr;

  bool is_new_turn = false;
  bool got_new_obj_card = false;

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
