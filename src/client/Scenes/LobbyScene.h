#ifndef PROJECT_LOBBYSCENE_H
#define PROJECT_LOBBYSCENE_H

#include "Scene.h"
#include "client/NetworkedData/PlayerData.h"
#include "gamelib/NetworkedData/Players.h"
#include <enet/enet.h>
#include <soloud.h>
#include <soloud_wav.h>

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
  ~LobbyScene() override = default;

  void init() override;
  void networkConnected() override;
  void networkDisconnected() override;
  void networkDataReceived(const enet_uint8* data, size_t data_size) override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;

 private:
  // Network data handlers
  void receivedLobbyInfo(DataShare& received_data);
  void newClientConnected(DataShare& received_data);
  void clientChangedReady(DataShare& received_data);
  void serverStartsGame(DataShare& received_data);

  /* My info & timers */
  int lobby_id = -1;
  int my_player_index = -1;
  double game_countdown = 5.0;

  /* States */
  bool has_connected = false;
  bool can_change_ready_state = true;
  bool should_start_game = false;
  bool is_playing_countdown = false;

  /* Engine Functions */
  Menu main_menu;
  Localisation localiser;

  /* Players and Sprites */
  LobbyPlayer* players[4] = { nullptr, nullptr, nullptr, nullptr };
  LobbySprites lobby_sprites;

  /* Sound */
  FileHandler file_handler;
  SoLoud::Wav player_join;
  SoLoud::Wav player_disconnect;
  SoLoud::Wav player_ready;
  SoLoud::Wav player_unready;
  SoLoud::Wav countdown;
};

#endif // PROJECT_LOBBYSCENE_H
