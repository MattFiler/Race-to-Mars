#include "MenuScene.h"

/* Initialise the scene */
void MenuScene::init()
{
  main_menu.addMenuSprite("MAIN_MENU/background.jpg");
  main_menu.addMenuItem("MENU_NEWGAME");
  main_menu.addMenuItem("MENU_QUIT");
}

/* Handles connecting to the server */
void MenuScene::networkConnected() {}

/* Handles disconnecting from the server */
void MenuScene::networkDisconnected() {}

/* Handles receiving data from the server */
void MenuScene::networkDataReceived(const enet_uint8* data, size_t data_size) {}

/* Handles key inputs */
void MenuScene::keyHandler(const ASGE::SharedEventData data)
{
  auto event = static_cast<const ASGE::KeyEvent*>(data.get());

  if (event->action == ASGE::KEYS::KEY_PRESSED)
  {
    if (event->key == ASGE::KEYS::KEY_BACKSPACE)
    {
      if (username.size() != 0)
      {
        username.pop_back();
      }
    }
    else
    {
      username += static_cast<char>(event->key);
    }
  }

  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));

  if (main_menu.itemWasSelected(keys))
  {
    if (main_menu.selectedItemWas("MENU_NEWGAME"))
    {
      debug_text.print("Swapping to game scene.");
      next_scene = game_global_scenes::IN_GAME;
    }
    else if (main_menu.selectedItemWas("MENU_QUIT"))
    {
      debug_text.print("Quitting the game.");
      next_scene = game_global_scenes::QUIT_GAME;
    }
  }
}

/* Handles mouse clicks */
void MenuScene::clickHandler(const ASGE::SharedEventData data)
{
  // auto click = static_cast<const ASGE::ClickEvent*>(data.get());
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
  renderer->renderText("MenuScene", 100, 100);
  renderer->renderText("Please enter your name:", 350, 350);
  renderer->renderText(username, 350, 450);
}