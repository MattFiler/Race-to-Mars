#ifndef PROJECT_MENUSCENE_H
#define PROJECT_MENUSCENE_H

#include "Scene.h"

class MenuScene : public Scene
{
 public:
  MenuScene() = default;
  ~MenuScene() = default;

  void keyHandler(const ASGE::SharedEventData data) override;
  game_global_scenes update(const ASGE::GameTime& game_time) override;
};

#endif // PROJECT_MENUSCENE_H
