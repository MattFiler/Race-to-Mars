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
#include <vector>

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
  // Misc specific functions split out from the core methods
  void createButtonsAndPopups();
  void chatMessageInput(const ASGE::KeyEvent* event);
  void playingInput();
  void issuePopupClicks();
  void playingClicksWhenActive(Vector2& mouse_pos);
  void playingClicksWhenActiveOrInactive(Vector2& mouse_pos);

  void updatePopups(const ASGE::GameTime& game_time);
  void updatePopupVisibility(const ASGE::GameTime& game_time);
  void updateStateSpecificCursor(const ASGE::GameTime& game_time);
  void updateButtonStates(const ASGE::GameTime& game_time);

  // Network functionality - this can be found in GameScene_Networking.cpp
  void serverEndsClientTurn(DataShare& received_data);
  void clientMovesPlayerToken(DataShare& received_data);
  void clientActionPointsUpdated(DataShare& received_data);
  void serverSyncsCardInfo(DataShare& received_data);
  void serverSyncsPositionInfo(DataShare& received_data);

  // Debug output - disabled in release mode
  void debugOutput();

  // Content
  GameBoard board;
  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };
  SceneUI ui_manager;

  bool new_turn = true;

  // Ability trackers
  bool is_new_turn = false;
  bool got_new_obj_card = false;
  bool got_chicken_card = false;
  bool got_new_obj_this_turn = false;
  bool rolled_dice_this_turn = false;
  bool free_player_movement = false;
  bool used_item_this_turn = false;

  // Win/Lose State
  bool lost_game = false;
  bool won_game = false;
  bool game_over_timer_started = false;
  double game_over_timer = 0;

  // More state trackers
  int update_item_card = 0;
  int new_item_card = -1;
  int good_comm_roll = 0;
  bool game_is_paused = false;
  double game_pause_timer = 0;

  // chat msg
  bool entering_msg = false;
  bool new_chat_msg = false;
  bool unread_msgs = false;
  std::string my_chat_msg;
  std::string received_chat_msg;
  std::vector<std::string> chat_messages;
  size_t max_messages = 32;
  size_t max_message_size = 26;

  // Positioning for cards
  CardOffsets card_offsets;

  // Even more state trackers
  bool has_disconnected = false; // did local client disconnect?
  game_state current_state = game_state::PLAYING;
  // int current_progress_index = 0;
  bool current_scene_lock_active = false; // optional "scene lock" to freeze
                                          // client interaction - useful for the
                                          // end of a turn

  // Sounds
  FileHandler file_handler;
  SoLoud::Wav end_turn_sfx;
  SoLoud::Wav new_item_sfx;
  SoLoud::Wav dice_roll_sfx;
  SoLoud::Wav turn_ends_sfx;
  SoLoud::Wav move_counter_sfx;
  SoLoud::Wav option_disabled_sfx;
  SoLoud::Wav ap_assigned_sfx;
};
#endif // PROJECT_GAMESCENE_H
