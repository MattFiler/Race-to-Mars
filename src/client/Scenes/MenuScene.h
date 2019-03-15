#ifndef PROJECT_MENUSCENE_H
#define PROJECT_MENUSCENE_H

#include "../UI/Menu.h"
#include "Scene.h"

class MenuScene : public Scene
{
 public:
  MenuScene() = default;
  ~MenuScene() = default;

  void init() override;

  void networkConnected() override;
  void networkDisconnected() override;
  void networkDataReceived(const enet_uint8* data, size_t data_size) override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;

 private:
  Menu main_menu;
  std::string username;
};

#endif // PROJECT_MENUSCENE_H
