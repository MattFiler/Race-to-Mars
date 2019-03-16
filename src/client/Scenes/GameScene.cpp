#include "GameScene.h"
#include <client/game.h>
#include <gamelib/Packet.h>

/* Initialise the scene */
void GameScene::init() {}

/* Handles connecting to the server */
void GameScene::networkConnected() {}

/* Handles disconnecting from the server */
void GameScene::networkDisconnected() {}

/* Handles receiving data from the server */
void GameScene::networkDataReceived(const enet_uint8* data, size_t data_size)
{
  // handle packet here | decide what data was passed in and call the correct
  // function[s] etc...
}

/* Handles key inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));

  if (keys.keyReleased("Activate"))
  {
    debug_text.print("Swapping to menu scene.");
    next_scene = game_global_scenes::MAIN_MENU;
  }
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());
  if (m_board.checkForClicks(Vector2(static_cast<float>(click->xpos),
                                     static_cast<float>(click->ypos))))
  {
    debug_text.print("CLICK WAS IN A ROOM");
  }
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  int test = 300;
  Packet packet(test);
  // need to lock mutex and push back packet to network client to be able to
  // push to server.

  return next_scene;
}

/* Render function */
void GameScene::render()
{
  m_board.render();
  renderer->renderText("GameScene", 100, 100);
}