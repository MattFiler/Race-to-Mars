#include "SceneManager.h"
#include "GameScene.h"
#include "LobbyScene.h"
#include "MenuScene.h"
#include "TutorialScene.h"
#include <exception>

/* Set the current scene */
bool SceneManager::setCurrentScene(game_global_scenes new_scene)
{
  if (new_scene == current_scene_id || new_scene == game_global_scenes::DEFAULT)
  {
    return true;
  }
  current_scene_id = new_scene;
  switch (new_scene)
  {
    case game_global_scenes::MAIN_MENU:
    {
      current_scene = dynamic_cast<Scene*>(new MenuScene());
      break;
    }
    case game_global_scenes::TUTORIAL:
    {
      current_scene = dynamic_cast<Scene*>(new TutorialScene());
      break;
    }
    case game_global_scenes::LOBBY:
    {
      current_scene = dynamic_cast<Scene*>(new LobbyScene());
      break;
    }
    case game_global_scenes::IN_GAME:
    {
      current_scene = dynamic_cast<Scene*>(new GameScene());
      break;
    }
    case game_global_scenes::QUIT_GAME:
    {
      return false;
    }
    default:
    {
      debug_text.print("@setCurrentScene - Attempted to access a scene which "
                       "is unhandled.",
                       2);
      break;
    }
  }
  current_scene->init();
  return true;
}

/* Update current scene */
bool SceneManager::update(const ASGE::GameTime& game_time)
{
  return setCurrentScene(current_scene->update(game_time));
}
