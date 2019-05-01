#ifndef PROJECT_MENUSCENE_H
#define PROJECT_MENUSCENE_H

#include "Scene.h"
#include <enet/enet.h>

class MenuScene : public Scene
{
 public:
  MenuScene() = default;
  ~MenuScene() override = default;

  void init() override;
  void networkConnected() override{};
  void networkDisconnected() override{};
  void networkDataReceived(const enet_uint8* data, size_t data_size) override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override{};
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;

 private:
  Menu main_menu;
};

#endif // PROJECT_MENUSCENE_H
