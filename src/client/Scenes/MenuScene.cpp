#include "MenuScene.h"

void MenuScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (keys.keyReleased("Activate"))
  {
    debug_text.print("Swapping to game scene.");
    next_scene = game_global_scenes::IN_GAME;
  }
}

game_global_scenes MenuScene::update(const ASGE::GameTime& game_time)
{
  return next_scene;
}