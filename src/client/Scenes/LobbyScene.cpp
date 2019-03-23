#include "LobbyScene.h"
#include "../Core/ServiceLocator.h"
#include "../NetworkConnection/NetworkConnection.h"
#include "../Players/AllPlayers.h"
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
  main_menu.addMenuSprite("LOBBY/background.jpg");
  this_is_you = new ScaledSprite("data/UI/LOBBY/this_is_you.png");
  game_countdown_ui = new ScaledSprite("data/UI/LOBBY/starting_3_notext.png");
  for (int i = 0; i < 4; i++)
  {
    ready_marker[i] = new ScaledSprite("data/UI/LOBBY/ready.png");
  }

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
        lobby_id = received_data.content[0];
        my_player_index = received_data.content[9];

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

      debug_text.print("A player connected to the lobby!");
      break;
    }
    case data_roles::PLAYER_DISCONNECTED_FROM_LOBBY:
    {
      // Forget them!
      players[received_data.content[0]].performDisconnect();

      debug_text.print("A player disconnected from the lobby!");
      break;
    }
    case data_roles::PLAYER_CHANGED_LOBBY_READY_STATE:
    {
      // Player is now ready/unready when they weren't before
      if (received_data.content[1] != my_player_index)
      {
        players[received_data.content[1]].is_ready =
          static_cast<bool>(received_data.content[0]);
        debug_text.print("A player changed their ready state!");
      }
      break;
    }
    case data_roles::SERVER_STARTS_GAME:
    {
      // The server has told us this lobby should start
      for (int i = 0; i < 4; i++)
      {
        players[i].is_ready = true;
      }
      should_start_game = true;
      can_change_ready_state = false;
      debug_text.print("Server told us to start the game!");
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
  if (keys.keyReleased("Ready Up") && can_change_ready_state)
  {
    // Alert everyone we're ready or unready (can't unready after all are ready)
    players[my_player_index].is_ready = !players[my_player_index].is_ready;
    Locator::getClient()->sendData(
      data_roles::PLAYER_CHANGED_LOBBY_READY_STATE,
      static_cast<int>(players[my_player_index].is_ready),
      my_player_index,
      lobby_id);
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
  if (keys.keyReleased("Debug Skip Readyup"))
  {
    // DEBUG ONLY LOCAL GAME START
    // should_start_game = true;
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
  if (should_start_game)
  {
    game_countdown -= game_time.delta.count() / 1000;
    if (game_countdown <= 0.0)
    {
      next_scene = game_global_scenes::IN_GAME;
      should_start_game = false;
    }
  }
  return next_scene;
}

/* Render function */
void LobbyScene::render()
{
  main_menu.render();

  for (int i = 0; i < 4; i++)
  {
    float this_pos = static_cast<float>(320 * i);
    Locator::getPlayers()
      ->getPlayer(players[i].current_class)
      ->getLobbySprite()
      ->getSprite()
      ->xPos(this_pos);
    if (i == my_player_index)
    {
      this_is_you->xPos(this_pos);
    }
    renderer->renderSprite(*Locator::getPlayers()
                              ->getPlayer(players[i].current_class)
                              ->getLobbySprite()
                              ->getSprite());
    // renderer->renderText(Locator::getPlayers()->getPlayer(players[i].current_class)->getFriendlyName(),
    // 50 + this_pos, 530, 0.5);
    if (players[i].is_ready)
    {
      ready_marker[i]->xPos(this_pos);
      renderer->renderSprite(*ready_marker[i]->getSprite());
    }
  }
  renderer->renderSprite(*this_is_you->getSprite());

  if (should_start_game)
  {
    renderer->renderSprite(*game_countdown_ui->getSprite());
    renderer->renderText(
      localiser.getString("LOBBY_COUNTDOWN_" +
                          std::to_string(static_cast<int>(game_countdown) + 1)),
      45,
      678,
      ASGE::COLOURS::WHITE);
  }
}