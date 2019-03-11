#include "MenuScene.h"

/* Initialise the scene */
void MenuScene::init() {}

/* Handles key inputs */
void MenuScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (keys.keyReleased("Activate"))
  {
    debug_text.print("Swapping to game scene.");
    next_scene = game_global_scenes::IN_GAME;
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
  renderer->renderText("MenuScene", 100, 100);
}