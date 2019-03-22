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

  // Request to connect to a lobby
  Locator::getClient()->sendData(data_roles::PLAYER_REQUESTS_LOBBY_CONNECTION,
                                 0);
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
    case data_roles::PLAYER_REQUESTS_LOBBY_CONNECTION:
    {
      int connection_count = 0;
      for (int i = 0; i < 4; i++)
      {
        if (players[i].has_connected)
        {
          connection_count++;
        }
      }
      Locator::getClient()->sendData(
        data_roles::PLAYER_LOBBY_REQUEST_ACCEPTED,
        connection_count,
        static_cast<int>(players[0].current_class),
        static_cast<int>(players[1].current_class),
        static_cast<int>(players[2].current_class));
      break;
    }
    case data_roles::PLAYER_LOBBY_REQUEST_ACCEPTED:
    {
      if (!has_connected)
      {
        // Fill in data we already know about clients in this lobby
        for (int i = 0; i < 3; i++)
        {
          if (received_data.content[i + 1] != -1)
          {
            players[i].current_class =
              static_cast<player_classes>(received_data.content[i + 1]);
            players[i].has_connected = true;
          }
        }

        // Find an empty slot for us to enter
        for (int i = 0; i < 4; i++)
        {
          if (!players[i].has_connected)
          {
            my_player_index = i;
            debug_text.print("YOUR PLAYER SLOT IS " +
                             std::to_string(my_player_index));
            break;
          }
        }
        if (my_player_index == -1)
        {
          // The lobby was full - ideally we'll handle this a little nicer than
          // throwing an exception!
          throw std::runtime_error("LOBBY IS FULL");
        }

        // Find us a class
        bool full_up_classes[4] = { false, false, false, false };
        for (int i = 0; i < 4; i++)
        {
          if (players[i].has_connected)
          {
            full_up_classes[static_cast<int>(players[i].current_class)] = true;
          }
        }
        for (int i = 0; i < 4; i++)
        {
          if (!full_up_classes[i])
          {
            players[my_player_index].current_class =
              static_cast<player_classes>(i);
            break;
          }
        }

        debug_text.print(
          "YOU ARE CLASS " +
          std::to_string(players[my_player_index].current_class));

        // Notify all clients in the lobby that we've connected
        Locator::getClient()->sendData(
          data_roles::PLAYER_CONNECTED_TO_LOBBY,
          static_cast<int>(players[my_player_index].current_class));

        // Update all class sprites (test)
        for (int i = 0; i < 4; i++)
        {
          TEST_updatePlayerIcon(i);
        }

        has_connected = true;
      }
      break;
    }
    case data_roles::PLAYER_CONNECTED_TO_LOBBY:
    {
      for (int i = 0; i < 4; i++)
      {
        if (!players[i].has_connected)
        {
          players[i].has_connected = true;
          players[i].current_class =
            static_cast<player_classes>(received_data.content[0]);
          TEST_updatePlayerIcon(i);

          debug_text.print("THEY ARE CLASS " +
                           std::to_string(players[i].current_class));
          break;
        }
      }

      // Update all class sprites (test)
      for (int i = 0; i < 4; i++)
      {
        TEST_updatePlayerIcon(i);
      }

      debug_text.print("A player connected to the lobby!");
      break;
    }
    case data_roles::PLAYER_DISCONNECTED_FROM_LOBBY:
    {
      for (int i = 0; i < 4; i++)
      {
        if (static_cast<int>(players[i].current_class) ==
            received_data.content[0])
        {
          players[i].performDisconnect();
          TEST_updatePlayerIcon(i);
          break;
        }
      }

      // Update all class sprites (test)
      for (int i = 0; i < 4; i++)
      {
        TEST_updatePlayerIcon(i);
      }

      debug_text.print("A player disconnected from the lobby!");
      break;
    }
    case data_roles::PLAYER_CHANGED_LOBBY_READY_STATE:
    {
      for (int i = 0; i < 4; i++)
      {
        if (static_cast<int>(players[i].current_class) ==
            received_data.content[1])
        {
          players[i].is_ready = static_cast<bool>(received_data.content[0]);
          break;
        }
      }
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
    renderer->renderSprite(*players[i].player_class_sprite->getSprite());
  }
}

/* update player icon sprite */
void LobbyScene::TEST_updatePlayerIcon(int player_index)
{
  std::string sprite_path = "";
  switch (players[player_index].current_class)
  {
    case player_classes::COMMUNICATIONS:
    {
      Communications player;
      sprite_path = player.getCounterSpritePath();
      break;
    }
    case player_classes::MEDIC:
    {
      Medic player;
      sprite_path = player.getCounterSpritePath();
      break;
    }
    case player_classes::PILOT:
    {
      Pilot player;
      sprite_path = player.getCounterSpritePath();
      break;
    }
    case player_classes::ENGINEER:
    {
      Engineer player;
      sprite_path = player.getCounterSpritePath();
      break;
    }
    default:
    {
      sprite_path = "data/icon.jpg";
    }
  }
  delete players[player_index].player_class_sprite;
  players[player_index].player_class_sprite = new ScaledSprite(sprite_path);
  players[player_index].player_class_sprite->yPos(450);
  players[player_index].player_class_sprite->xPos(
    static_cast<float>(100 + (100 * player_index)));
  players[player_index].player_class_sprite->height(50);
  players[player_index].player_class_sprite->width(50);
}