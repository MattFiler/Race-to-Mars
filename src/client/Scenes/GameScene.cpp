#include "GameScene.h"

/* Initialise the scene */
void GameScene::init() {}

/* Handles connecting to the server */
void GameScene::networkConnected() {}

/* Handles disconnecting from the server */
void GameScene::networkDisconnected() {}

/* Handles receiving data from the server */
void GameScene::networkDataReceived(const enet_uint8* data, size_t data_size) {}

/* Handles key inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  // auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  if (m_board.didClickOnInteractable(
        Vector2(Locator::getCursor()->getPosition().x / GameResolution::scale,
                Locator::getCursor()->getPosition().y / GameResolution::scale,
                false)))
  {
    debug_text.print("Clicked on an interactable part of the board!");
  }
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  Locator::getCursor()->setCursorActive(m_board.isHoveringOverInteractable(
    Vector2(Locator::getCursor()->getPosition().x / GameResolution::scale,
            Locator::getCursor()->getPosition().y / GameResolution::scale,
            false)));
  return next_scene;
}

/* Render function */
void GameScene::render()
{
  m_board.render();
  renderer->renderText("GameScene", 100, 100);
}