#include "GameScene.h"
#include <client/game.h>
#include <gamelib/Packet.h>

/* Initialise the scene */
void GameScene::init()
{
  m_board_menu.addMenuSprite("BOARD/background.jpg");
}

/* Handles connecting to the server */
void GameScene::networkConnected() {}

/* Handles disconnecting from the server */
void GameScene::networkDisconnected() {}

/* Handles receiving data from the server */
void GameScene::networkDataReceived(const enet_uint8* data, size_t data_size)
{
  // Recreate packet
  Packet data_packet(data, data_size);
  // handle packet here | decide what data was passed in and call the correct
  // function[s] etc...
  int test_val4;
  data_packet >> test_val4;
  if (test_val4 == 5)
  {
    test_int2 += test_val4;
  }
}

/* Handles key inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (keys.keyReleased("Back"))
  {
    debug_text.print("Swapping to menu scene.");
    next_scene = game_global_scenes::MAIN_MENU;
  }

  auto event = static_cast<const ASGE::KeyEvent*>(data.get());
  if (event->action == ASGE::KEYS::KEY_PRESSED)
  {
    if (event->key == ASGE::KEYS::KEY_P)
    {
    }
    if (event->key == ASGE::KEYS::KEY_O)
    {
      test_val = true;
    }
  }
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  // auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);
  if (m_board.isHoveringOverInteractable(mouse_pos))
  {
    ShipRoom this_room = m_board.getClickedInteractable(mouse_pos);
    debug_text.print("Clicked on an interactable part of the board!");
    debug_text.print("CLICKED: " + this_room.getName());
  }
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  int test = 5;
  if (test_val)
  {
    Packet packet;
    packet << test;
    Locator::getClient()->getPacketQueue()->push(packet);
    test_val = false;
  }

  Locator::getCursor()->setCursorActive(m_board.isHoveringOverInteractable(
    Vector2(Locator::getCursor()->getPosition().x,
            Locator::getCursor()->getPosition().y)));
  return next_scene;
}

/* Render function */
void GameScene::render()
{
  m_board_menu.render();
  m_board.render();
  // debug packet code
  renderer->renderText(std::to_string(test_int2), 100, 100);
}