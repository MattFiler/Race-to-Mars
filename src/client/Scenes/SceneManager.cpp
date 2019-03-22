#include "SceneManager.h"
#include "GameScene.h"
#include "LobbyScene.h"
#include "MenuScene.h"
#include <exception>

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
      // TODO: Implement quitting!
      throw std::runtime_error("QUITTING GAME ISN'T SUPPORTED YET LOL");
    }
    default:
    {
      debug_text.print("Attempted to access a scene which is unhandled.");
      break;
    }
  }
  current_scene->init();
}

void SceneManager::update(const ASGE::GameTime& game_time)
{
  setCurrentScene(current_scene->update(game_time));
}
