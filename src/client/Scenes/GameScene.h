#ifndef PROJECT_GAMESCENE_H
#define PROJECT_GAMESCENE_H

#include "../UI/PlayerCounter.h"
#include "Scene.h"

class GameScene : public Scene
{
 public:
  GameScene() { debug_text.print("entered game scene."); };
  ~GameScene() = default;

  void init() override;

  void keyHandler(const ASGE::SharedEventData data) override;
  void clickHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
  void render() override;

 private:
  PlayerCounter test_counter;
};

#endif // PROJECT_GAMESCENE_H
