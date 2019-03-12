#ifndef PROJECT_SCENEMANAGER_H
#define PROJECT_SCENEMANAGER_H

#include "../Debug/DebugText.h"
#include "Scene.h"
#include "gamelib/Constants.h"
#include <Engine/GameTime.h>

class SceneManager
{
 public:
  SceneManager() = default;
  ~SceneManager() = default;

  void setCurrentScene(game_global_scenes new_scene);
  game_global_scenes getCurrentScene() { return current_scene_id; };

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
