#include "client/Scenes/GameScene.h"
#include "GameScene.h"
#include "client/Core/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "gamelib/NetworkedData/MessageTypes.h"
#include "gamelib/NetworkedData/NetworkedData.h"

#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>
#include <gamelib/Packet.h>

/* Initialise the scene */
void GameScene::init()
{
  pause_menu.addMenuSprite("INGAME_UI/pause_bg.jpg");
  pause_menu.addMenuItem("MENU_CONTINUE");
  pause_menu.addMenuItem("MENU_QUIT");
  // m_deck.setup();

  // Get a reference to the client lobby data array
  for (int i = 0; i < 4; i++)
  {
    players[i] = &Locator::getPlayers()->players[i];
    if (players[i]->is_this_client)
    {
      my_player_index = i;
    }
  }

  game_sprites.background = new ScaledSprite("UI/BOARD/background.jpg");
  game_sprites.active_player_marker = new ScaledSprite("UI/INGAME_UI/"
                                                       "active_player_tab.png");
  game_sprites.active_player_marker->yPos(-180.0f);
  game_sprites.inactive_player_marker = new ScaledSprite("UI/INGAME_UI/"
                                                         "inactive_player_tab_"
                                                         "singlesprite.png");
  game_sprites.progress_meter = new ScaledSprite("UI/INGAME_UI/"
                                                 "progress_bar.png");
  game_sprites.progress_marker = new ScaledSprite("UI/INGAME_UI/"
                                                  "progress_marker_padded.png");
  game_sprites.progress_marker->yPos(89.0f); // increment this as we progress
  game_sprites.sync_overlay = new ScaledSprite("UI/INGAME_UI/syncing.png");
  game_sprites.disconnect_overlay = new ScaledSprite("UI/INGAME_UI/"
                                                     "syncing_notext.png");
}

/* Handles connecting to the server */
void GameScene::networkConnected()
{
  has_disconnected = false;
}

/* Handles disconnecting from the server */
void GameScene::networkDisconnected()
{
  has_disconnected = true;
}

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
    case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
    {
      // Forget them!
      players[received_data.content[0]]->performDisconnect();

      // Uhh so yeah we should probably work in a handler for this :)
      // E.G. save scores in the background? , don't allow another to replace? ,
      // etc.
      debug_text.print("A PLAYER DISCONNECTED FROM THE LOBBY - WHAT THE HELL "
                       "DO WE DO NOW?!",
                       1);
      break;
    }
    case data_roles::CLIENT_CONNECTED_TO_LOBBY:
    {
      // A player that's not us connected to the lobby, update our info
      if (received_data.content[0] != my_player_index)
      {
        players[received_data.content[0]]->is_ready = true;
        players[received_data.content[0]]->current_class =
          static_cast<player_classes>(received_data.content[2]);
      }

      // And yeah, should really handle this too! Might get complicated with
      // scores, etc - maybe don't allow it?
      debug_text.print("A NEW PLAYER CONNECTED TO THE LOBBY - AGAIN, WHAT THE "
                       "HELL DO WE DO!?!?",
                       1);
      break;
    }
    case data_roles::SERVER_ENDED_CLIENT_TURN:
    {
      // Update active player flag
      for (int i = 0; i < 4; i++)
      {
        players[i]->is_active = (received_data.content[1] == i);
      }

      // Re-sync progress index every turn
      current_progress_index = received_data.content[2];

      // Re-sync issue cards every turn
      int active_issue_cards[5] = { received_data.content[3],
                                    received_data.content[4],
                                    received_data.content[5],
                                    received_data.content[6],
                                    received_data.content[7] };

      if (received_data.content[12])
      {
        board.setActiveIssueCards(active_issue_cards,
                                  static_cast<bool>(received_data.content[12]));
      }

      // Pull a new objective card if required.
      if (current_progress_index % 3 == 0 && current_progress_index != 0)
      {
        board.setActiveObjectiveCard(
          received_data.content[8 + my_player_index]);
        debug_text.print(
          "Adding obj card for client " + std::to_string(my_player_index) +
          "of type: " +
          std::to_string(received_data.content[8 + my_player_index]));
      }

      // End the scene lock
      current_scene_lock_active = false;
      debug_text.print("The server ended the current go, and passed "
                       "active-ness to client " +
                       std::to_string(received_data.content[1]) + ".");
      break;
    }
    case data_roles::CLIENT_MOVING_PLAYER_TOKEN:
    {
      if (received_data.content[0] != my_player_index)
      {
        // Move a player token if its not ours (local lag is gross!)
        Locator::getPlayers()
          ->getPlayer(players[received_data.content[0]]->current_class)
          ->setPos(Vector2(static_cast<float>(received_data.content[1]),
                           static_cast<float>(received_data.content[2])));
        debug_text.print("A player on the server moved their counter! We're "
                         "gonna do that too.");
      }
      break;
    }
    case data_roles::CLIENT_ACTION_POINTS_CHANGED:
    {
      // Update another player's action point count
      players[received_data.content[0]]->action_points =
        received_data.content[1];
      debug_text.print("A player spent or received action points.");
      break;
    }
    default:
    {
      debug_text.print("An unhandled data packet was received, of type " +
                         std::to_string(received_data.role) + "!",
                       1);
      break;
    }
  }
}

/* Handles key inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));

  switch (current_state)
  {
    case game_state::LOOKING_AT_PLAYER_INFO:
    {
      // --
      break;
    }
    case game_state::PLAYING:
    {
      if (keys.keyReleased("Back") && !current_scene_lock_active)
      {
        current_state = game_state::LOCAL_PAUSE;
        debug_text.print("Opening pause menu.");
      }
      if (keys.keyReleased("End Turn") && players[my_player_index]->is_active)
      {
        Locator::getClient()->sendData(data_roles::CLIENT_WANTS_TO_END_TURN,
                                       my_player_index);
        current_scene_lock_active = true;
        debug_text.print("Requesting to end my go!!");
      }
      if (keys.keyReleased("Debug Spend AP") &&
          players[my_player_index]->is_active)
      {
        // Debug: change my action points to 10
        Locator::getClient()->sendData(
          data_roles::CLIENT_ACTION_POINTS_CHANGED, my_player_index, 10);
        debug_text.print("Debug: changing my action points to 10!");
      }
      break;
    }
    case game_state::LOCAL_PAUSE:
    {
      if (pause_menu.itemWasSelected(keys))
      {
        if (pause_menu.selectedItemWas("MENU_CONTINUE"))
        {
          current_state = game_state::PLAYING;
          debug_text.print("Closing pause menu.");
        }
        else if (pause_menu.selectedItemWas("MENU_QUIT"))
        {
          // Alert everyone we're leaving and then return to menu
          Locator::getClient()->sendData(
            data_roles::CLIENT_DISCONNECTING_FROM_LOBBY, my_player_index);
          next_scene = game_global_scenes::MAIN_MENU;
          debug_text.print("Returning to main menu and disconnecting from "
                           "lobby.");
        }
      }
      break;
    }
  }
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  if (click->action == ASGE::E_MOUSE_CLICK)
  {
    Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                                Locator::getCursor()->getPosition().y);
    if (!current_scene_lock_active && players[my_player_index]->is_active)
    {
      switch (m_board.isHoveringOverInteractable(mouse_pos))
      {
        // Clicked on an objective card
        case hovered_type::HOVERED_OVER_OBJECTIVE_CARD:
        {
          ObjectiveCard* this_card = m_board.getClickedObjectiveCard(mouse_pos);
          debug_text.print("Clicked on an interactable part of the board!");
          debug_text.print("CLICKED: OBJECTIVE CARD " +
                           std::to_string(this_card->getCardID()));

          break;
        }

        // Clicked on an issue card
        case hovered_type::HOVERED_OVER_ISSUE_CARD:
        {
          IssueCard* this_card = m_board.getClickedIssueCard(mouse_pos);
          debug_text.print("Clicked on an interactable part of the board!");
          debug_text.print("CLICKED: ISSUE CARD " +
                           std::to_string(this_card->getCardID()));

          break;
        }

        // Clicked within a room on the ship
        case hovered_type::HOVERED_OVER_SHIP_ROOM:
        {
          // Get new movement position
          Vector2 new_pos =
            m_board.getClickedInteractableRoom(mouse_pos).getPosForPlayer(
              players[my_player_index]->current_class);

          // Move, and let everyone know we're moving
          Locator::getClient()->sendData(data_roles::CLIENT_MOVING_PLAYER_TOKEN,
                                         my_player_index,
                                         static_cast<int>(new_pos.x),
                                         static_cast<int>(new_pos.y));
          Locator::getPlayers()
            ->getPlayer(players[my_player_index]->current_class)
            ->setPos(new_pos);
          debug_text.print("Moving my player token to this room.");
          break;
        }

        // Didn't click on anything
        default:
        {
          return;
        }
      }
    }
  }
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  // Update cards if required
  board.updateActiveIssueCards();
  board.updateActiveObjectiveCard();

  if (players[my_player_index]->is_active)
  {
    // If we're not syncing, handle hover sprite update.
    if (!current_scene_lock_active)
    {
      Locator::getCursor()->setCursorActive(
        m_board.isHoveringOverInteractable(
          Vector2(Locator::getCursor()->getPosition().x,
                  Locator::getCursor()->getPosition().y)) !=
        hovered_type::DID_NOT_HOVER_OVER_ANYTHING);
    }
  }
  else
  {
    // It isn't our go, but we might be able to still do stuff, or need to
    // update bits?
    Locator::getCursor()->setCursorActive(false);
  }
  return next_scene;
}

/* Render function */
void GameScene::render()
{
  switch (current_state)
  {
    case game_state::LOOKING_AT_PLAYER_INFO:
    {
      // Yes clang, I do want to use a switch case for its intended purpose!
      [[clang::fallthrough]];
    }
    case game_state::PLAYING:
    {
      // Board and background
      renderer->renderSprite(*game_sprites.background->getSprite());
      board.render();

      float active_marker_pos = -180.0f;
      for (int i = 0; i < 4; i++)
      {
        float this_pos = static_cast<float>(180 * i);

        // Player tab
        Locator::getPlayers()
          ->getPlayer(players[i]->current_class)
          ->getGameTabSprite()
          ->yPos(this_pos);
        renderer->renderSprite(*Locator::getPlayers()
                                  ->getPlayer(players[i]->current_class)
                                  ->getGameTabSprite()
                                  ->getSprite());
        renderer->renderText(std::to_string(players[i]->action_points),
                             225,
                             static_cast<int>(this_pos + 100),
                             ASGE::COLOURS::WHITE);

        // log position for active player marker.
        if (players[i]->is_active)
        {
          active_marker_pos = this_pos;
        }
      }

      // Activity markers
      renderer->renderSprite(*game_sprites.inactive_player_marker->getSprite());
      game_sprites.active_player_marker->yPos(active_marker_pos);
      renderer->renderSprite(*game_sprites.active_player_marker->getSprite());

      // Progress meters
      renderer->renderSprite(*game_sprites.progress_meter->getSprite());
      game_sprites.progress_marker->yPos(
        static_cast<float>(current_progress_index * 50));
      renderer->renderSprite(*game_sprites.progress_marker->getSprite());

      break;
    }
    case game_state::LOCAL_PAUSE:
    {
      pause_menu.render();
    }
  }
  // If active, render the "scene lock" overlay (cuts out interaction while
  // syncing)
  if (current_scene_lock_active)
  {
    renderer->renderSprite(*game_sprites.sync_overlay->getSprite());
  }
  if (has_disconnected)
  {
    renderer->renderSprite(*game_sprites.disconnect_overlay->getSprite());
  }

  // client debugging
  if (debug_text.enabled)
  {
    renderer->renderText("IS_ACTIVE: " +
                           std::to_string(players[my_player_index]->is_active),
                         10,
                         30,
                         0.5);
    renderer->renderText(
      "CURRENT_CLASS: " +
        std::to_string(players[my_player_index]->current_class),
      10,
      50,
      0.5);
    renderer->renderText(
      "HAS_CONNECTED: " +
        std::to_string(players[my_player_index]->has_connected),
      10,
      70,
      0.5);
    renderer->renderText("IS_READY: " +
                           std::to_string(players[my_player_index]->is_ready),
                         10,
                         90,
                         0.5);
    renderer->renderText(
      "ACTION_POINTS: " +
        std::to_string(players[my_player_index]->action_points),
      10,
      110,
      0.5);
    renderer->renderText("PRESS M TO FINISH TURN", 10, 130, 0.5);
    renderer->renderText("PRESS L TO DEBUG TEST ACTION POINTS", 10, 150, 0.5);
  }
}