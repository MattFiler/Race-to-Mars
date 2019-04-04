#ifndef PROJECT_SCENE_H
#define PROJECT_SCENE_H

#include "client/Core/ServiceLocator.h"
#include "client/KeyHandler/KeyHandler.h"
#include "client/UI/Menu.h"
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

  virtual void init(){};

  virtual void networkConnected(){};
  virtual void networkDisconnected(){};
  virtual void networkDataReceived(const enet_uint8* data, size_t data_size){};

  virtual void keyHandler(const ASGE::SharedEventData data){};
  virtual void clickHandler(const ASGE::SharedEventData data){};
  virtual game_global_scenes update(const ASGE::GameTime& game_time)
  {
    return next_scene;
  };
  virtual void render(){};

 protected:
  ASGE::Input* input = nullptr;
  ASGE::Renderer* renderer = nullptr;
  SoLoud::Soloud* audio = nullptr;
  KeyHandler keys;
  DebugText debug_text;

  game_global_scenes next_scene = game_global_scenes::DEFAULT;
};

#endif // PROJECT_SCENE_H
