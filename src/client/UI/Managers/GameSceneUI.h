#ifndef PROJECT_GAMESCENEUI_H
#define PROJECT_GAMESCENEUI_H

/*
 *
 * This class will be used to hold all of the UI in for a scene.
 *
 */

#include "client/UI/Managers/PopupManager.h"
#include "client/UI/ScaledSprite.h"
#include <Engine/GameTime.h>
#include <Engine/Renderer.h>
#include <vector>

class SceneUI
{
 public:
  SceneUI();
  ~SceneUI();

  ScaledSprite* createSprite(const std::string& id, const std::string& path);
  ScaledSprite* getSprite(const std::string& id);

  PopupManager& popups() { return popup_manager; };

  void update(const ASGE::GameTime& game_time);
  void render();

 private:
  ASGE::Renderer* renderer = nullptr;

  std::vector<std::string> sprite_ids;
  std::vector<ScaledSprite*> sprites;

  PopupManager popup_manager;
};

#endif // PROJECT_GAMESCENEUI_H
