#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H

#include "../Core/ServiceLocator.h"
#include "../KeyHandler/KeyHandler.h"
#include "../Debug/DebugText.h"
#include "gamelib/Constants.h"
#include <Engine/GameTime.h>

class Scene
{
 public:
  Scene()
  {
    input = Locator::getInput();
    renderer = Locator::getRenderer();
    audio = Locator::getAudio();
  };
  virtual ~Scene() = default;

  virtual void keyHandler(const ASGE::SharedEventData data) { return; };
  virtual void clickHandler(const ASGE::SharedEventData data) { return; };
  virtual game_global_scenes update(const ASGE::GameTime& game_time)
  {
    return next_scene;
  };
  virtual void render() { return; };

 protected:
  ASGE::Input* input = nullptr;
  ASGE::Renderer* renderer = nullptr;
  SoLoud::Soloud* audio = nullptr;
  KeyHandler keys;
    DebugText debug_text;

  game_global_scenes next_scene = game_global_scenes::DEFAULT;
};

#endif // PROJECT_SCENE_H
