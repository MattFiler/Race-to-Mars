#include "GameScene.h"

/* Initialise the scene */
void GameScene::init()
{
  test_counter.setSprite("200x200.png");
  test_counter.setPosition(Vector2(200, 200));
}

/* Handles key inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  // auto click = static_cast<const ASGE::ClickEvent*>(data.get());
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  return next_scene;
}

/* Render function */
void GameScene::render()
{
  test_counter.render();
  renderer->renderText("GameScene", 100, 100);
}