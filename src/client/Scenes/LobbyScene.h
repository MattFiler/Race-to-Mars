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
  ScaledSprite* player_class_sprite = new ScaledSprite("data/icon.jpg");
  std::string player_class_text = "";
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
  void TEST_updatePlayerIcon(int player_index);

  Menu main_menu;
  int my_player_index = -1; // the index of me in the player array
  LobbyPlayer players[4];
  bool has_connected = false; // have i connected to the lobby?
};

#endif // PROJECT_LOBBYSCENE_H