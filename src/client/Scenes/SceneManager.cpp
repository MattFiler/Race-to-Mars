#include "SceneManager.h"

#include "GameScene.h"
#include "MenuScene.h"

/* Set the current scene */
void SceneManager::setCurrentScene(game_global_scenes new_scene)
{
  if (new_scene == current_scene_id || new_scene == game_global_scenes::DEFAULT)
  {
    return;
  }
  current_scene_id = new_scene;
  switch (new_scene)
  {
    case game_global_scenes::MAIN_MENU:
    {
      current_scene = dynamic_cast<Scene*>(new MenuScene());
      break;
    }
    case game_global_scenes::IN_GAME:
    {
      current_scene = dynamic_cast<Scene*>(new GameScene());
      break;
    }
    default:
    {
      debug_text.print("Attempted to access a scene which is unhandled.");
      break;
    }
  }
  current_scene->init();
}