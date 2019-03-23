#ifndef PROJECT_LOBBYSCENE_H
#define PROJECT_LOBBYSCENE_H

#include "Scene.h"
#include "gamelib/NetworkedData/Players.h"
#include <enet/enet.h>

// All useful info for a player in the lobby
struct LobbyPlayer
{
  void performDisconnect()
  {
    has_connected = false;
    is_ready = false;
    current_class = player_classes::UNASSIGNED;
  }
  bool has_connected = false;
  bool is_ready = false;
  player_classes current_class = player_classes::UNASSIGNED;
};

// All sprites for the lobby ui
struct LobbySprites
{
  ScaledSprite* this_is_you = nullptr;
  ScaledSprite* game_countdown_ui = nullptr;
  ScaledSprite* ready_prompt_marker[4] = { nullptr, nullptr, nullptr, nullptr };
  ScaledSprite* ready_marker[4] = { nullptr, nullptr, nullptr, nullptr };
};

class LobbyScene : public Scene
{
 public:
  LobbyScene() = default;
  ~LobbyScene() = default;

  void init() override;
  void networkConnected() override;
  void networkDisconnected() override;
  void networkDataReceived(const enet_uint8* data, size_t data_size) override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;

 private:
  int lobby_id = -1;
  int my_player_index = -1;
  double game_countdown = 5.0;

  bool has_connected = false;
  bool can_change_ready_state = true;
  bool should_start_game = false;

  Menu main_menu;
  Localisation localiser;

  LobbyPlayer players[4];
  LobbySprites lobby_sprites;
};

#endif // PROJECT_LOBBYSCENE_H
