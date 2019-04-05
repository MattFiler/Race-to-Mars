#include "LobbyScene.h"
#include "../Core/ServiceLocator.h"
#include "../NetworkConnection/NetworkConnection.h"
#include "../Players/AllPlayers.h"
#include "../Players/ClientPlayer.h"
#include <exception>

/* Initialise the scene */
void LobbyScene::init()
{
  // Load some required sprites
  main_menu.addMenuSprite("LOBBY/background.jpg");
  lobby_sprites.this_is_you = new ScaledSprite("data/UI/LOBBY/this_is_you.png");
  lobby_sprites.game_countdown_ui = new ScaledSprite("data/UI/LOBBY/"
                                                     "starting_3_notext_alt."
                                                     "png");

  // Get a reference to the client lobby data array to update
  for (int i = 0; i < 4; i++)
  {
    players[i] = &Locator::getPlayers()->players[i];
  }

  // Request lobby info
  DataShare new_share = DataShare(data_roles::CLIENT_REQUESTS_TO_JOIN_LOBBY);
  new_share.add(0);
  Locator::getNetworkInterface()->sendData(new_share);
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
  DataShare received_data;
  data_packet >> received_data;

  // Handle all relevant data packets for this scene
  switch (received_data.getType())
  {
    // Server gives the lobby information we require, utilise it!
    case data_roles::SERVER_GIVES_LOBBY_INFO:
    {
      receivedLobbyInfo(received_data);
      break;
    }

    // A new client has connected to the lobby, update our records with their
    // data
    case data_roles::CLIENT_CONNECTED_TO_LOBBY:
    {
      newClientConnected(received_data);
      break;
    }

    // A client has disconnected from the lobby
    case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
    {
      // Forget them!
      players[received_data.retrieve(0)]->performDisconnect();

      debug_text.print("A player disconnected from the lobby!");
      break;
    }

    // A client has readied/unreadied in the lobby
    case data_roles::CLIENT_CHANGED_LOBBY_READY_STATE:
    {
      clientChangedReady(received_data);
      break;
    }

    // The server has told us to start the game
    case data_roles::SERVER_STARTS_GAME:
    {
      serverStartsGame(received_data);
      break;
    }

    // Anything else is unhandled
    default:
    {
      debug_text.print("An unhandled data packet was received", 1);
      break;
    }
  }
}

/* Received lobby info */
void LobbyScene::receivedLobbyInfo(DataShare& received_data)
{
  // Don't need info if we've already connected!
  if (has_connected)
  {
    return;
  }

  // Fill out our known local player data from the server
  lobby_id = received_data.retrieve(0);
  my_player_index = received_data.retrieve(9);
  for (int i = 0; i < 4; i++)
  {
    players[i]->current_class =
      static_cast<player_classes>(received_data.retrieve(i + 1));
    players[i]->is_ready =
      static_cast<player_classes>(received_data.retrieve(i + 5));
    if (players[i]->current_class != player_classes::UNASSIGNED)
    {
      players[i]->has_connected = true;
    }
    players[i]->is_this_client = (i == my_player_index);
  }

  // Notify all clients in the lobby that we've connected
  DataShare new_share = DataShare(data_roles::CLIENT_CONNECTED_TO_LOBBY);
  new_share.add(my_player_index);
  new_share.add(players[my_player_index]->is_ready);
  new_share.add(players[my_player_index]->current_class);
  Locator::getNetworkInterface()->sendData(new_share);

  debug_text.print("We synced to the lobby!", -1);
  has_connected = true;
}

/* New client connected */
void LobbyScene::newClientConnected(DataShare& received_data)
{
  if (received_data.retrieve(0) == my_player_index)
  {
    return;
  }

  // A player that's not us connected to the lobby, update our info
  players[received_data.retrieve(0)]->is_ready =
    static_cast<bool>(received_data.retrieve(1));
  players[received_data.retrieve(0)]->current_class =
    static_cast<player_classes>(received_data.retrieve(2));
  debug_text.print("A player connected to the lobby!");
}

/* Client is ready/unready when they weren't before */
void LobbyScene::clientChangedReady(DataShare& received_data)
{
  // Player is now ready/unready when they weren't before
  if (received_data.retrieve(1) != my_player_index)
  {
    players[received_data.retrieve(1)]->is_ready =
      static_cast<bool>(received_data.retrieve(0));
    debug_text.print("A player changed their ready state!");
  }
}

/* Server told us to start the game */
void LobbyScene::serverStartsGame(DataShare& received_data)
{
  // The server has told us this lobby should start
  for (int i = 0; i < 4; i++)
  {
    players[i]->is_ready = true;
    players[i]->is_active = false; // make sure we have no active conflicts
  }
  Locator::getPlayers()->joined_in_progress =
    static_cast<bool>(received_data.retrieve(1));
  players[received_data.retrieve(0)]->is_active = true; // client to start
  should_start_game = true;
  can_change_ready_state = false;
  debug_text.print("Server told us to start the game!");
}

/* Handles key inputs */
void LobbyScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));
  if (keys.keyReleased("Ready Up") && can_change_ready_state)
  {
    // Alert everyone we're ready or unready (can't unready after all are ready)
    players[my_player_index]->is_ready = !players[my_player_index]->is_ready;
    DataShare new_share =
      DataShare(data_roles::CLIENT_CHANGED_LOBBY_READY_STATE);
    new_share.add(static_cast<int>(players[my_player_index]->is_ready));
    new_share.add(my_player_index);
    new_share.add(lobby_id);
    Locator::getNetworkInterface()->sendData(new_share);
  }
  if (keys.keyReleased("Back") && !should_start_game)
  {
    // Alert everyone we're leaving
    DataShare new_share =
      DataShare(data_roles::CLIENT_DISCONNECTING_FROM_LOBBY);
    new_share.add(my_player_index);
    Locator::getNetworkInterface()->sendData(new_share);

    // Leave
    debug_text.print("Swapping to menu scene.");
    next_scene = game_global_scenes::MAIN_MENU;
  }

  /* DEBUGGING */
  if (debug_text.enabled && keys.keyReleased("Debug Skip Readyup"))
  {
    // DEBUG ONLY LOCAL GAME START
    int player_count = 0;
    for (int i = 0; i < 4; i++)
    {
      if (players[i]->has_connected)
      {
        player_count++;
      }
    }
    if (player_count > 1)
    {
      debug_text.print("Debug Skip Readyup only functions with one player in "
                       "the lobby.");
      return;
    }
    players[my_player_index]->is_active = true;
    should_start_game = true;
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
  if (has_connected && lobby_sprites.ready_marker[0] == nullptr)
  {
    for (int i = 0; i < 4; i++)
    {
      lobby_sprites.ready_marker[i] = new ScaledSprite("UI/LOBBY/ready.png");
      std::string prompt_path = "UI/LOBBY/ready_prompt_notext.png";
      if (i == my_player_index)
      {
        prompt_path = "UI/LOBBY/ready_prompt.png";
      }
      lobby_sprites.ready_prompt_marker[i] = new ScaledSprite(prompt_path);
    }
  }

  if (should_start_game)
  {
    // Game intro countdown
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

    // Position player sprite and render
    Locator::getPlayers()
      ->getPlayer(players[i]->current_class)
      ->getLobbySprite()
      ->xPos(this_pos);
    if (i == my_player_index)
    {
      lobby_sprites.this_is_you->xPos(this_pos); // Mark which one we are
    }
    renderer->renderSprite(*Locator::getPlayers()
                              ->getPlayer(players[i]->current_class)
                              ->getLobbySprite()
                              ->getSprite());

    // Render ready-up prompt when appropriate
    if (lobby_sprites.ready_marker[i] != nullptr)
    {
      if (players[i]->is_ready)
      {
        lobby_sprites.ready_marker[i]->xPos(this_pos);
        renderer->renderSprite(*lobby_sprites.ready_marker[i]->getSprite());
      }
      else
      {
        lobby_sprites.ready_prompt_marker[i]->xPos(this_pos);
        renderer->renderSprite(
          *lobby_sprites.ready_prompt_marker[i]->getSprite());
      }
    }
  }

  // Render our marker when connected
  if (my_player_index != -1)
  {
    renderer->renderSprite(*lobby_sprites.this_is_you->getSprite());
  }

  // Render game countdown when active
  if (should_start_game)
  {
    renderer->renderSprite(*lobby_sprites.game_countdown_ui->getSprite());
    renderer->renderText(
      localiser.getString("LOBBY_COUNTDOWN_" +
                          std::to_string(static_cast<int>(game_countdown + 1))),
      45,
      698,
      1,
      ASGE::COLOURS::WHITE);
  }
}