#include "LobbyScene.h"
#include "../Core/ServiceLocator.h"
#include "../NetworkConnection/NetworkConnection.h"
#include "../Players/ClientPlayer.h"
#include <exception>

// This bit is gonna get a refactor - so labelled TEST for now :)
#include "../Players/CommunicationsPlayer.h"
#include "../Players/EngineerPlayer.h"
#include "../Players/MedicPlayer.h"
#include "../Players/PilotPlayer.h"
// End of mess (look for function TEST_ below where it continues)

/* Initialise the scene */
void LobbyScene::init()
{
  main_menu.addMenuSprite("MAIN_MENU/background.jpg");

  // Request lobby info
  Locator::getClient()->sendData(data_roles::CLIENT_REQUESTS_LOBBY_INFO, 0);
}

/* Handles connecting to the server */
void LobbyScene::networkConnected() {}

/* Handles disconnecting from the server */
void LobbyScene::networkDisconnected() {}

/* Handles receiving data from the server */
void LobbyScene::networkDataReceived(const enet_uint8* data, size_t data_size)
{
  // Recreate packet
  Packet data_packet(data, data_size);
  NetworkedData received_data;
  data_packet >> received_data;

  // Handle all relevant data packets for this scene
  switch (received_data.role)
  {
    case data_roles::SERVER_GIVES_LOBBY_INFO:
    {
      if (!has_connected)
      {
        // Fill out our known local player data from the server
        for (int i = 0; i < 4; i++)
        {
          players[i].current_class =
            static_cast<player_classes>(received_data.content[i + 1]);
          players[i].is_ready =
            static_cast<player_classes>(received_data.content[i + 5]);
        }
        my_player_index = received_data.content[9];

        // Update all sprites and descriptions (wip)
        for (int i = 0; i < 4; i++)
        {
          TEST_updatePlayerIcon(i);
        }
        debug_text.print("THIS CLIENT IS: " +
                         players[my_player_index].player_class_text);

        // Notify all clients in the lobby that we've connected
        Locator::getClient()->sendData(data_roles::PLAYER_CONNECTED_TO_LOBBY,
                                       my_player_index,
                                       players[my_player_index].is_ready,
                                       players[my_player_index].current_class);
        debug_text.print("We synced to the lobby!");

        has_connected = true;
      }
      break;
    }
    case data_roles::PLAYER_CONNECTED_TO_LOBBY:
    {
      // A player that's not us connected to the lobby, update our info
      if (received_data.content[0] != my_player_index)
      {
        players[received_data.content[0]].is_ready =
          static_cast<bool>(received_data.content[1]);
        players[received_data.content[0]].current_class =
          static_cast<player_classes>(received_data.content[2]);
      }

      // Update all sprites and descriptions (wip)
      for (int i = 0; i < 4; i++)
      {
        TEST_updatePlayerIcon(i);
      }

      debug_text.print("A player connected to the lobby!");
      break;
    }
    case data_roles::PLAYER_DISCONNECTED_FROM_LOBBY:
    {
      // Forget them!
      players[received_data.content[0]].performDisconnect();

      // Update all sprites and descriptions (wip)
      for (int i = 0; i < 4; i++)
      {
        TEST_updatePlayerIcon(i);
      }

      debug_text.print("A player disconnected from the lobby!");
      break;
    }
    case data_roles::PLAYER_CHANGED_LOBBY_READY_STATE:
    {
      // Player is now ready/unready when they weren't before
      players[received_data.content[1]].is_ready =
        static_cast<bool>(received_data.content[0]);
      debug_text.print("A player changed their ready state!");
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
void LobbyScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (keys.keyReleased("Ready Up"))
  {
    // Alert everyone we're ready or unready
    players[my_player_index].is_ready = !players[my_player_index].is_ready;
    Locator::getClient()->sendData(
      data_roles::PLAYER_CHANGED_LOBBY_READY_STATE,
      static_cast<int>(players[my_player_index].is_ready),
      static_cast<int>(players[my_player_index].current_class));
  }
  if (keys.keyReleased("Back"))
  {
    // Alert everyone we're leaving
    Locator::getClient()->sendData(
      data_roles::PLAYER_DISCONNECTED_FROM_LOBBY,
      static_cast<int>(players[my_player_index].current_class));

    // Leave
    debug_text.print("Swapping to menu scene.");
    next_scene = game_global_scenes::MAIN_MENU;
  }
}

/* Handles mouse clicks */
void LobbyScene::clickHandler(const ASGE::SharedEventData data)
{
  // auto click = static_cast<const ASGE::ClickEvent*>(data.get());
}

/* Update function */
game_global_scenes LobbyScene::update(const ASGE::GameTime& game_time)
{
  return next_scene;
}

/* Render function */
void LobbyScene::render()
{
  main_menu.render();
  for (int i = 0; i < 4; i++)
  {
    // renderer->renderSprite(*players[i].player_class_sprite->getSprite());
    renderer->renderText(
      players[i].player_class_text, 100 + (100 * i), 530, 0.5);
    renderer->renderText(
      "THIS IS YOU", 100 + (100 * my_player_index), 420, 0.5);
  }
}

/* update player icon sprite */
void LobbyScene::TEST_updatePlayerIcon(int player_index)
{
  std::string sprite_path = "";
  std::string player_text = "";
  switch (players[player_index].current_class)
  {
    case player_classes::COMMUNICATIONS:
    {
      Communications player;
      sprite_path = player.getCounterSpritePath();
      player_text = player.getFriendlyName();
      break;
    }
    case player_classes::MEDIC:
    {
      Medic player;
      sprite_path = player.getCounterSpritePath();
      player_text = player.getFriendlyName();
      break;
    }
    case player_classes::PILOT:
    {
      Pilot player;
      sprite_path = player.getCounterSpritePath();
      player_text = player.getFriendlyName();
      break;
    }
    case player_classes::ENGINEER:
    {
      Engineer player;
      sprite_path = player.getCounterSpritePath();
      player_text = player.getFriendlyName();
      break;
    }
    default:
    {
      sprite_path = "data/UI/PLAYER_COUNTERS/placeholder.png";
      player_text = "EMPTY";
    }
  }
  delete players[player_index].player_class_sprite;
  players[player_index].player_class_sprite = new ScaledSprite(sprite_path);
  players[player_index].player_class_sprite->yPos(450);
  players[player_index].player_class_sprite->xPos(
    static_cast<float>(100 + (100 * player_index)));
  players[player_index].player_class_sprite->height(50);
  players[player_index].player_class_sprite->width(50);
  players[player_index].player_class_text = player_text;
}