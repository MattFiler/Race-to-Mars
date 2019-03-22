#include "client/Scenes/GameScene.h"
#include "gamelib/NetworkedData/MessageTypes.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include <client/game.h>
#include <gamelib/Packet.h>

/* Initialise the scene */
void GameScene::init()
{
  m_board_menu.addMenuSprite("BOARD/background.jpg");
  m_deck.initDecks();
  m_deck.shuffleDecks();
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
  NetworkedData received_data;
  data_packet >> received_data;

  // Handle all relevant data packets for this scene
  switch (received_data.role)
  {
    case data_roles::PLAYER_ASSIGNED_ACTION_POINTS:
    {
      debug_text.print("A player assigned action points!");
      debug_text.print("Card the points were assigned to: " +
                       std::to_string(received_data.content[0]));
      debug_text.print("Number of points assigned: " +
                       std::to_string(received_data.content[1]));
      debug_text.print("Was the card completed? (0=no, 1=yes): " +
                       std::to_string(received_data.content[2]));
      break;
    }
    default:
    {
      debug_text.print("An unhandled data packet was received");
      break;
    }
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
  if (keys.keyReleased("Debug Test"))
  {
    test_val = true;
  }
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
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
}