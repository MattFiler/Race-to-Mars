#ifndef PROJECT_GAMESCENEUI_H
#define PROJECT_GAMESCENEUI_H

/*
 *
 * This class will be used to hold all of the UI in for a scene.
 *
 */

#include "client/UI/Managers/PopupManager.h"
#include "client/UI/Menu.h"
#include "client/UI/ScaledSprite.h"
#include <Engine/GameTime.h>
#include <Engine/Renderer.h>
#include <vector>

class SceneUI
{
 public:
  SceneUI();
  ~SceneUI();

  ScaledSprite* createSprite(int id, const std::string& path);
  ScaledSprite* getSprite(int id);

  ClickableButton* createButton(int id, const std::string& path);
  ClickableButton* getButton(int id);

  Menu* createMenu();
  Menu* getMenu();

  PopupManager& popups() { return popup_manager; };

  void update(const ASGE::GameTime& game_time);
  void render();

 private:
  ASGE::Renderer* renderer = nullptr;

  std::vector<int> sprite_ids;
  std::vector<ScaledSprite*> sprites;

  std::vector<int> button_ids;
  std::vector<ClickableButton*> buttons;

  Menu* menu = nullptr;
  bool has_menu = false;

  PopupManager popup_manager;
};

#endif // PROJECT_GAMESCENEUI_H
