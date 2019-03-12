#include "GameScene.h"

/* Initialise the scene */
void GameScene::init() {}

/* Handles connecting to the server */
void networkConnected() {}

/* Handles disconnecting from the server */
void networkDisconnected() {}

/* Handles receiving data from the server */
void networkDataReceived(const enet_uint8* data, size_t data_size) {}

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
  m_players.render();
  renderer->renderText("GameScene", 100, 100);
}