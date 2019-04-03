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

  // Get a reference to the client lobby data array
  for (int i = 0; i < 4; i++)
  {
    players[i] = &Locator::getPlayers()->players[i];
    if (players[i]->is_this_client)
    {
      Locator::getPlayers()->my_player_index = i;
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
  game_sprites.issue_popup = new ScaledSprite("UI/INGAME_UI/new_issues_bg.png");
  game_sprites.objective_popup = new ScaledSprite("UI/INGAME_UI/"
                                                  "new_obj_bg.png");
  game_sprites.roll_dice = new ScaledSprite("UI/INGAME_UI/roll_btn.png");

  // If we joined in progress, request a data sync from the server
  if (Locator::getPlayers()->joined_in_progress)
  {
    Locator::getClient()->sendData(data_roles::CLIENT_REQUESTS_SYNC, 0);
  }
  // Else, manually position our player based on their starting room
  else
  {
    ShipRoom this_room = board.getRoom(static_cast<ship_rooms>(
      Locator::getPlayers()
        ->getPlayer(
          players[Locator::getPlayers()->my_player_index]->current_class)
        ->getStartingRoom()));

    // Get new movement position
    Vector2 new_pos = this_room.getPosForPlayer(
      players[Locator::getPlayers()->my_player_index]->current_class);

    // Move, and let everyone know we're moving
    Locator::getClient()->sendData(data_roles::CLIENT_MOVING_PLAYER_TOKEN,
                                   Locator::getPlayers()->my_player_index,
                                   static_cast<int>(this_room.getEnum()));
    Locator::getPlayers()
      ->getPlayer(
        players[Locator::getPlayers()->my_player_index]->current_class)
      ->setPos(new_pos);
    debug_text.print("Starting my player token in room '" +
                     this_room.getName() + "'.");
  }
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
    // A client has disconnected from the game
    case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
    {
      // Forget them!
      players[received_data.content[0]]->performDisconnect();
      // While connecting/disconnecting works properly now, it might be nice to
      // implement some more validation to it - at the moment anyone can rejoin
      // and replace the original player. Similarly, it's untested if it works
      // with more than one player leaving.
      break;
    }
    // A new client has connected to the lobby
    case data_roles::CLIENT_CONNECTED_TO_LOBBY:
    {
      // A player that's not us connected to the lobby, update our info
      if (received_data.content[0] != Locator::getPlayers()->my_player_index)
      {
        players[received_data.content[0]]->is_ready = true;
        players[received_data.content[0]]->current_class =
          static_cast<player_classes>(received_data.content[2]);
      }
      break;
    }
    // The server has ended the current turn, update our game accordingly
    case data_roles::SERVER_ENDED_CLIENT_TURN:
    {
      // Update active player flag.
      for (int i = 0; i < 4; i++)
      {
        players[i]->is_active = (received_data.content[1] == i);
      }
      // Re-sync progress index every turn.
      Locator::getPlayers()->current_progress_index = received_data.content[2];
      // Re-sync issue cards every turn.
      if (received_data.content[12])
      {
        int active_issue_cards[5] = { received_data.content[3],
                                      received_data.content[4],
                                      received_data.content[5],
                                      received_data.content[6],
                                      received_data.content[7] };
        board.setActiveIssueCards(active_issue_cards,
                                  static_cast<bool>(received_data.content[12]));
      }
      // Pull a new objective card if required.
      if (Locator::getPlayers()->current_progress_index % 3 == 0 &&
          Locator::getPlayers()->current_progress_index != 0)
      {
        board.setActiveObjectiveCard(
          received_data.content[8 + Locator::getPlayers()->my_player_index]);
        debug_text.print(
          "Adding obj card for client " +
          std::to_string(Locator::getPlayers()->my_player_index) + "of type: " +
          std::to_string(
            received_data.content[8 + Locator::getPlayers()->my_player_index]));
      }
      // End the scene lock.
      current_scene_lock_active = false;
      debug_text.print("The server ended the current go, and passed "
                       "active-ness to client " +
                       std::to_string(received_data.content[1]) + ".");
      break;
    }
    // The active client has moved their player token, update it on our screen.
    case data_roles::CLIENT_MOVING_PLAYER_TOKEN:
    {
      if (received_data.content[0] != Locator::getPlayers()->my_player_index)
      {
        ShipRoom this_room =
          board.getRoom(static_cast<ship_rooms>(received_data.content[1]));

        // Get new movement position and move to it.
        Vector2 new_pos = this_room.getPosForPlayer(
          players[received_data.content[0]]->current_class);
        Locator::getPlayers()
          ->getPlayer(players[received_data.content[0]]->current_class)
          ->setPos(new_pos);
        debug_text.print("Moving player " +
                         std::to_string(received_data.content[0]) +
                         " to room '" + this_room.getName() + "'.");
      }
      break;
    }
    // The active client's action points have changed, update it for us.
    case data_roles::CLIENT_ACTION_POINTS_CHANGED:
    {
      // Update another player's action point count
      players[received_data.content[0]]->action_points =
        received_data.content[1];
      debug_text.print("A player spent or received action points.");
      break;
    }
    // If connecting to an in-progress game, the server needs to sync
    // information to us.
    case data_roles::SERVER_SYNCS_CARD_INFO:
    {
      // Sync issue cards.
      int issue_cards[5] = { received_data.content[0],
                             received_data.content[1],
                             received_data.content[2],
                             received_data.content[3],
                             received_data.content[4] };
      board.setActiveIssueCards(issue_cards, true); // should sync ship
                                                    // position here too
      debug_text.print("Sync: updated active issue cards.");
      for (int i = 0; i < 5; i++)
      {
        debug_text.print("Sync: issue card 1: " +
                         std::to_string(received_data.content[i]));
      }
      // Sync my objective card
      board.setActiveObjectiveCard(
        received_data.content[5 + Locator::getPlayers()->my_player_index]);
      debug_text.print(
        "Sync: updated my objective card to " +
        std::to_string(
          received_data.content[5 + Locator::getPlayers()->my_player_index]) +
        ".");
      // Sync action points.
      for (int i = 0; i < 4; i++)
      {
        players[i]->action_points = received_data.content[9 + i];
        debug_text.print("Sync: updated player " + std::to_string(i) +
                         "'s points to " +
                         std::to_string(players[i]->action_points) + ".");
      }
      break;
    }
    case data_roles::SERVER_SYNCS_POSITION_INFO:
    {
      for (int i = 0; i < 4; i++)
      {
        ShipRoom this_room =
          board.getRoom(static_cast<ship_rooms>(received_data.content[i]));
        // Get new movement position and move to it
        Vector2 new_pos = this_room.getPosForPlayer(players[i]->current_class);
        Locator::getPlayers()
          ->getPlayer(players[i]->current_class)
          ->setPos(new_pos);

        // Broadcast out the position of us to keep all clients informed
        if (i == Locator::getPlayers()->my_player_index)
        {
          Locator::getClient()->sendData(data_roles::CLIENT_MOVING_PLAYER_TOKEN,
                                         Locator::getPlayers()->my_player_index,
                                         static_cast<int>(this_room.getEnum()));
        }

        debug_text.print("Sync: moved player " + std::to_string(i) +
                         " to room '" + this_room.getName() + "'.");
      }
      // Sync ship board progress
      Locator::getPlayers()->current_progress_index = received_data.content[4];
      debug_text.print(
        "Sync: moved ship to position " +
        std::to_string(Locator::getPlayers()->current_progress_index));
      break;
    }
    case data_roles::CLIENT_CHANGE_PROGRESS_INDEX:
    {
      debug_text.print(
        "Changing current_progress index from:" +
        std::to_string(Locator::getPlayers()->current_progress_index));
      Locator::getPlayers()->current_progress_index = received_data.content[0];
      debug_text.print(
        ". to:" +
        std::to_string(Locator::getPlayers()->current_progress_index));
      break;
    }
    // Anything else is unhandled.
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
    case game_state::ISSUE_CARDS_POPUP:
    {
      if (keys.keyReleased("Back") && !is_new_turn)
      {
        current_state = game_state::PLAYING;
        debug_text.print("Closing issue card popup.");
      }
      break;
    }
    case game_state::OBJECTIVE_CARD_POPUP:
    {
      if (keys.keyReleased("Back") && !got_new_obj_card)
      {
        current_state = game_state::PLAYING;
        debug_text.print("Closing objective popup.");
      }
      break;
    }
    case game_state::IS_ROLLING_DICE:
    {
      break;
    }
    case game_state::PLAYING:
    {
      if (keys.keyReleased("Back") && !current_scene_lock_active)
      {
        current_state = game_state::LOCAL_PAUSE;
        debug_text.print("Opening pause menu.");
      }
      if (keys.keyReleased("End Turn") &&
          players[Locator::getPlayers()->my_player_index]->is_active)
      {
        Locator::getClient()->sendData(data_roles::CLIENT_WANTS_TO_END_TURN,
                                       Locator::getPlayers()->my_player_index);
        current_scene_lock_active = true;
        debug_text.print("Requesting to end my go!!");
      }
      if (keys.keyReleased("Debug Spend AP") &&
          players[Locator::getPlayers()->my_player_index]->is_active)
      {
        // Debug: change my action points to 10.
        Locator::getClient()->sendData(data_roles::CLIENT_ACTION_POINTS_CHANGED,
                                       Locator::getPlayers()->my_player_index,
                                       10);
        debug_text.print("Debug: changing my action points to 10!");
      }
      break;
    }
    case game_state::LOCAL_PAUSE:
    {
      if (!pause_menu.itemWasSelected(keys))
      {
        break;
      }
      if (pause_menu.selectedItemWas("MENU_CONTINUE"))
      {
        current_state = game_state::PLAYING;
        debug_text.print("Closing pause menu.");
      }
      else if (pause_menu.selectedItemWas("MENU_QUIT"))
      {
        // Alert everyone we're leaving and then return to menu
        Locator::getClient()->sendData(
          data_roles::CLIENT_DISCONNECTING_FROM_LOBBY,
          Locator::getPlayers()->my_player_index);
        next_scene = game_global_scenes::MAIN_MENU;
        debug_text.print("Returning to main menu and disconnecting from "
                         "lobby.");
      }
      break;
    }
  }
}

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  auto click = static_cast<const ASGE::ClickEvent*>(data.get());

  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);

  if (click->action != ASGE::E_MOUSE_CLICK)
  {
    return;
  }

  switch (current_state)
  {
    case game_state::ISSUE_CARDS_POPUP:
    {
      break;
    }
    case game_state::OBJECTIVE_CARD_POPUP:
    {
      break;
    }
    case game_state::IS_ROLLING_DICE:
    {
      break;
    }
    case game_state::PLAYING:
    {
      // Disallow interaction when scene lock is active
      if (current_scene_lock_active)
      {
        break;
      }

      /* WHEN CLIENT IS ACTIVE */
      if (players[Locator::getPlayers()->my_player_index]->is_active)
      {
        // Clicked within a room on the ship
        if (board.isHoveringOverRoom(mouse_pos))
        {
          ShipRoom this_room = board.getClickedRoom(mouse_pos);

          // Get new movement position
          Vector2 new_pos = this_room.getPosForPlayer(
            players[Locator::getPlayers()->my_player_index]->current_class);

          // Move, and let everyone know we're moving
          Locator::getClient()->sendData(data_roles::CLIENT_MOVING_PLAYER_TOKEN,
                                         Locator::getPlayers()->my_player_index,
                                         static_cast<int>(this_room.getEnum()));
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->setPos(new_pos);
          debug_text.print("Moving my player token to room '" +
                           this_room.getName() + "'.");
        }
      }

      /* WHEN CLIENT IS ACTIVE/INACTIVE */
      // Clicked on an objective card
      if (board.isHoveringOverObjectiveCard(mouse_pos))
      {
        clicked_obj_card = board.getClickedObjectiveCard(mouse_pos);
        current_state = game_state::OBJECTIVE_CARD_POPUP;
      }
      // Clicked on an issue card
      if (board.isHoveringOverIssueCard(mouse_pos))
      {
        clicked_issue_card = board.getClickedIssueCard(mouse_pos);
        current_state = game_state::ISSUE_CARDS_POPUP;
      }
    }
    case game_state::LOCAL_PAUSE:
    {
      break;
    }
  }
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  // Update cards if required & log result
  if (board.updateActiveIssueCards())
  {
    current_state = game_state::ISSUE_CARDS_POPUP;
    is_new_turn = true;
  }
  if (board.updateActiveObjectiveCard())
  {
    got_new_obj_card = true;
  }

  // Timeout new round popup after a given time
  if (is_new_turn && current_state == game_state::ISSUE_CARDS_POPUP)
  {
    popup_timer += game_time.delta.count() / 1000;

    // If popup has been on screen for 5 secs, hide it
    if (popup_timer > 5.0f)
    {
      if (got_new_obj_card)
      {
        current_state = game_state::OBJECTIVE_CARD_POPUP;
      }
      else
      {
        current_state = game_state::PLAYING; // Maybe go to rolling dice state
                                             // here?!
      }
      popup_timer = 0.0f;
      is_new_turn = false;
    }
  }

  // Do the same for the new objective card popup
  if (got_new_obj_card && current_state == game_state::OBJECTIVE_CARD_POPUP)
  {
    popup_timer += game_time.delta.count() / 1000;

    // If popup has been on screen for 5 secs, hide it
    if (popup_timer > 5.0f)
    {
      current_state = game_state::PLAYING; // Maybe go to rolling dice state
                                           // here?!
      popup_timer = 0.0f;
      got_new_obj_card = false;
    }
  }

  /* CURSOR UPDATE */
  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);

  // Always update to hover cursor for cards
  bool cursor_active = (board.isHoveringOverIssueCard(mouse_pos) ||
                        board.isHoveringOverObjectiveCard(mouse_pos));

  // If we're the active player, aren't syncing, and don't already have an
  // active player, check if we're hovering over a room
  if (players[Locator::getPlayers()->my_player_index]->is_active &&
      !current_scene_lock_active && !cursor_active)
  {
    cursor_active = board.isHoveringOverRoom(mouse_pos);
  }

  // Finally, actually update it
  Locator::getCursor()->setCursorActive(cursor_active);

  return next_scene;
}

/* Render function */
void GameScene::render()
{
  switch (current_state)
  {
    case game_state::ISSUE_CARDS_POPUP:
    {
      // Yes clang, I do want to use a switch case for its intended purpose!
      [[clang::fallthrough]];
    }
    case game_state::OBJECTIVE_CARD_POPUP:
    {
      [[clang::fallthrough]];
    }
    case game_state::IS_ROLLING_DICE:
    {
      [[clang::fallthrough]];
    }
    case game_state::PLAYING:
    {
      // Board and background
      renderer->renderSprite(*game_sprites.background->getSprite(),
                             render_order::PRIORITY_BACKGROUND);
      board.render(current_state);

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
                                  ->getSprite(),
                               render_order::PRIORITY_UI);

        // draw score if player is connected
        if (players[i]->current_class != player_classes::UNASSIGNED)
        {
          renderer->renderText(std::to_string(players[i]->action_points),
                               225,
                               static_cast<int>(this_pos + 100),
                               1,
                               ASGE::COLOURS::WHITE,
                               render_order::PRIORITY_TEXT);
        }

        // log position for active player marker.
        if (players[i]->is_active)
        {
          active_marker_pos = this_pos;
        }
      }

      // Activity markers
      renderer->renderSprite(*game_sprites.inactive_player_marker->getSprite(),
                             render_order::PRIORITY_UI);
      game_sprites.active_player_marker->yPos(active_marker_pos);
      renderer->renderSprite(*game_sprites.active_player_marker->getSprite(),
                             render_order::PRIORITY_UI_2);

      // Progress meters
      renderer->renderSprite(*game_sprites.progress_meter->getSprite(),
                             render_order::PRIORITY_UI);
      game_sprites.progress_marker->yPos(static_cast<float>(
        ((Locator::getPlayers()->current_progress_index + 3.5) * 30)));
      renderer->renderSprite(*game_sprites.progress_marker->getSprite(),
                             render_order::PRIORITY_UI);

      // Roll Dice Btn
      if (current_state == game_state::PLAYING)
      {
        game_sprites.roll_dice->getSprite()->colour(ASGE::COLOURS::GREY);
      }
      else if (current_state == game_state::IS_ROLLING_DICE)
      {
        game_sprites.roll_dice->getSprite()->colour();
      }
      renderer->renderSprite(*game_sprites.roll_dice->getSprite(),
                             render_order::PRIORITY_UI_2);

      // If card popup is active, render it too
      if (current_state == game_state::ISSUE_CARDS_POPUP)
      {
        renderer->renderSprite(*game_sprites.issue_popup->getSprite(),
                               render_order::PRIORITY_UI_3);
      }
      else if (current_state == game_state::OBJECTIVE_CARD_POPUP)
      {
        renderer->renderSprite(*game_sprites.objective_popup->getSprite(),
                               render_order::PRIORITY_UI_3);
      }

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
    renderer->renderSprite(*game_sprites.sync_overlay->getSprite(),
                           render_order::PRIORITY_OVERLAYS);
  }
  if (has_disconnected)
  {
    renderer->renderSprite(*game_sprites.disconnect_overlay->getSprite(),
                           render_order::PRIORITY_OVERLAYS);
  }

  // client debugging
  if (debug_text.enabled)
  {
    renderer->renderText(
      "IS_ACTIVE: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->is_active),
      10,
      30,
      0.5,
      ASGE::COLOURS::WHITE,
      render_order::PRIORITY_TEXT);
    renderer->renderText(
      "CURRENT_CLASS: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->current_class),
      10,
      50,
      0.5,
      ASGE::COLOURS::WHITE,
      render_order::PRIORITY_TEXT);
    renderer->renderText(
      "HAS_CONNECTED: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->has_connected),
      10,
      70,
      0.5,
      ASGE::COLOURS::WHITE,
      render_order::PRIORITY_TEXT);
    renderer->renderText(
      "IS_READY: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->is_ready),
      10,
      90,
      0.5,
      ASGE::COLOURS::WHITE,
      render_order::PRIORITY_TEXT);
    renderer->renderText(
      "ACTION_POINTS: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->action_points),
      10,
      110,
      0.5,
      ASGE::COLOURS::WHITE,
      render_order::PRIORITY_TEXT);
    renderer->renderText("PRESS M TO FINISH TURN",
                         10,
                         130,
                         0.5,
                         ASGE::COLOURS::WHITE,
                         render_order::PRIORITY_TEXT);
    renderer->renderText("PRESS L TO DEBUG TEST ACTION POINTS",
                         10,
                         150,
                         0.5,
                         ASGE::COLOURS::WHITE,
                         render_order::PRIORITY_TEXT);
  }
}