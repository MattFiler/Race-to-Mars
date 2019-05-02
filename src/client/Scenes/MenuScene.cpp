#include "MenuScene.h"

/* Initialise the scene */
void MenuScene::init()
{
  main_menu.addMenuSprite("MAIN_MENU/background.jpg");
  main_menu.addMenuItem("MENU_NEWGAME");
  main_menu.addMenuItem("MENU_TUTORIAL");
  main_menu.addMenuItem("MENU_QUIT");
}

/* Handles key inputs */
void MenuScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (main_menu.itemWasSelected(keys))
  {
    if (main_menu.selectedItemWas("MENU_NEWGAME"))
    {
      debug_text.print("Entering lobby.");
      next_scene = game_global_scenes::LOBBY;
    }
    else if (main_menu.selectedItemWas("MENU_TUTORIAL"))
    {
      debug_text.print("Entering tutorial slideshow.");
      next_scene = game_global_scenes::TUTORIAL;
    }
    else if (main_menu.selectedItemWas("MENU_QUIT"))
    {
      debug_text.print("Quitting the game.");
      next_scene = game_global_scenes::QUIT_GAME;
    }
  }
}

/* Update function */
game_global_scenes MenuScene::update(const ASGE::GameTime& game_time)
{
  return next_scene;
}

/* Render function */
void MenuScene::render()
{
  main_menu.render();
}