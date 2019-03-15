#ifndef PROJECT_SCENEMANAGER_H
#define PROJECT_SCENEMANAGER_H

#include "Scene.h"
#include "gamelib/Constants.h"
#include "gamelib/Debug/DebugText.h"
#include <Engine/GameTime.h>
#include <enet/enet.h>

class SceneManager
{
 public:
  SceneManager() = default;
  ~SceneManager() = default;

  void setCurrentScene(game_global_scenes new_scene);
  game_global_scenes getCurrentScene() { return current_scene_id; };

  // Call appropriate networking functions
  void networkConnected() { current_scene->networkConnected(); };
  void networkDisconnected() { current_scene->networkDisconnected(); };
  void networkDataReceived(const enet_uint8* data, size_t data_size)
  {
    current_scene->networkDataReceived(data, data_size);
  };

  // Call appropriate engine functions
  void keyHandler(const ASGE::SharedEventData data)
  {
    current_scene->keyHandler(data);
  };
  void clickHandler(const ASGE::SharedEventData data)
  {
    current_scene->clickHandler(data);
  };
  void update(const ASGE::GameTime& game_time)
  {
    setCurrentScene(current_scene->update(game_time));
  };
  void render() { current_scene->render(); };

 private:
  game_global_scenes current_scene_id = game_global_scenes::DEFAULT;
  Scene* current_scene;
  DebugText debug_text;
};

#endif // PROJECT_SCENEMANAGER_H
