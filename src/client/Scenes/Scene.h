#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H

#include "../Core/ServiceLocator.h"
#include "../KeyHandler/KeyHandler.h"
#include "gamelib/Constants.h"
#include "gamelib/Debug/DebugText.h"
#include <Engine/GameTime.h>
#include <Engine/Renderer.h>
#include <enet/enet.h>

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

  virtual void init() { return; };

  virtual void networkConnected() { return; };
  virtual void networkDisconnected() { return; };
  virtual void networkDataReceived(const enet_uint8* data, size_t data_size)
  {
    return;
  };

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
