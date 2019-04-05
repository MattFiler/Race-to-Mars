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

/* Create */
GameScene::GameScene()
{
  debug_text.print("entered game scene.");
}

/* Destroy! */
GameScene::~GameScene()
{
  // Destroy stuff here
}

/* Initialise the scene */
void GameScene::init()
{
  // Create pause menu
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

  // Create all required sprites
  game_sprites.background = new ScaledSprite("UI/BOARD/background.jpg");
  game_sprites.active_player_marker = new ScaledSprite("UI/INGAME_UI/"
                                                       "active_player_tab.png");
  game_sprites.active_player_marker->yPos(-180.0f);
  game_sprites.your_player_marker = new ScaledSprite("UI/INGAME_UI/"
                                                     "your_tab.png");
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
  for (int i = 0; i < 6; i++)
  {
    game_sprites.popup_card_shadows[i] =
      new ScaledSprite("UI/INGAME_UI/cards_" + std::to_string(i) + ".png");
  }

  // Create popup title sprites
  issue_card_popup.createSprite("UI/INGAME_UI/new_issues_bg.png");
  objective_card_popup.createSprite("UI/INGAME_UI/new_obj_bg.png");
  dice_roll_popup.createSprite("UI/INGAME_UI/dice_roll_bg.png");

  // Position button for new turn
  end_turn_btn.setPos(Vector2(1042, 626));

  // Issue popup card placeholder
  ScaledSprite& card_placeholder = issue_card_popup.createSprite("UI/"
                                                                 "CARD_IMAGES/"
                                                                 "PLACEHOLDER_"
                                                                 "5.png");
  card_placeholder.setPos(card_offsets.issue_popup_start);
  card_placeholder.setDims(Vector2(card_offsets.issue_popup_size.x * 5,
                                   card_offsets.issue_popup_size.y));

  for (int i = 0; i < 5; i++)
  {
    // Create buttons for issue card popup
    ClickableButton& new_btn = issue_card_popup.createButton("UI/INGAME_UI/"
                                                             "assign_ap_button_"
                                                             "noshadow.png");
    new_btn.setPos(
      card_offsets.issue_popup_ap_btn_start +
      (card_offsets.issue_popup_ap_btn_offset * static_cast<float>(i)));
    new_btn.setActive(false);

    // Create point overlays for issue card popup
    ScaledSprite& new_sprite = issue_card_popup.createSprite("UI/INGAME_UI/"
                                                             "score_overlay_"
                                                             "cutout.png");
    new_sprite.setPos(
      card_offsets.issue_popup_start +
      (card_offsets.issue_popup_offset * static_cast<float>(i)));
    new_sprite.setDims(card_offsets.issue_popup_size);
    new_sprite.hide();
  }

  // If we joined in progress, request a data sync from the server
  if (Locator::getPlayers()->joined_in_progress)
  {
    DataShare new_share = DataShare(data_roles::CLIENT_REQUESTS_SYNC);
    new_share.add(0);
    Locator::getNetworkInterface()->sendData(new_share);
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
    DataShare new_share = DataShare(data_roles::CLIENT_MOVING_PLAYER_TOKEN);
    new_share.add(Locator::getPlayers()->my_player_index);
    new_share.add(static_cast<int>(this_room.getEnum()));
    Locator::getNetworkInterface()->sendData(new_share);
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
  DataShare received_data;
  data_packet >> received_data;

  // Handle all relevant data packets for this scene
  switch (received_data.getType())
  {
    // A client has disconnected from the game
    case data_roles::CLIENT_DISCONNECTING_FROM_LOBBY:
    {
      // Forget them!
      players[received_data.retrieve(0)]->performDisconnect();
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
      if (received_data.retrieve(0) != Locator::getPlayers()->my_player_index)
      {
        players[received_data.retrieve(0)]->is_ready = true;
        players[received_data.retrieve(0)]->current_class =
          static_cast<player_classes>(received_data.retrieve(2));
      }
      break;
    }
    // The server has ended the current turn, update our game accordingly
    case data_roles::SERVER_ENDED_CLIENT_TURN:
    {
      // Update active player flag.
      for (int i = 0; i < 4; i++)
      {
        players[i]->is_active = (received_data.retrieve(1) == i);
      }
      // Re-sync progress index every turn.
      Locator::getPlayers()->current_progress_index = received_data.retrieve(2);
      // Re-sync issue cards every turn.
      if (received_data.retrieve(12))
      {
        int active_issue_cards[5] = { received_data.retrieve(3),
                                      received_data.retrieve(4),
                                      received_data.retrieve(5),
                                      received_data.retrieve(6),
                                      received_data.retrieve(7) };
        board.setActiveIssueCards(
          active_issue_cards, static_cast<bool>(received_data.retrieve(12)));
      }
      // Pull a new objective card if required.
      if (Locator::getPlayers()->current_progress_index % 3 == 0 &&
          Locator::getPlayers()->current_progress_index != 0 &&
          !got_new_obj_this_turn)
      {
        board.setActiveObjectiveCard(
          received_data.retrieve(8 + Locator::getPlayers()->my_player_index));
        debug_text.print(
          "Adding obj card for client " +
          std::to_string(Locator::getPlayers()->my_player_index) + "of type: " +
          std::to_string(received_data.retrieve(
            8 + Locator::getPlayers()->my_player_index)));
      }
      // End the scene lock.
      current_scene_lock_active = false;
      debug_text.print("The server ended the current go, and passed "
                       "active-ness to client " +
                       std::to_string(received_data.retrieve(1)) + ".");
      break;
    }
    // The active client has moved their player token, update it on our screen.
    case data_roles::CLIENT_MOVING_PLAYER_TOKEN:
    {
      if (received_data.retrieve(0) == Locator::getPlayers()->my_player_index)
      {
        break;
      }

      ShipRoom this_room =
        board.getRoom(static_cast<ship_rooms>(received_data.retrieve(1)));

      // Get new movement position and move to it.
      Vector2 new_pos = this_room.getPosForPlayer(
        players[received_data.retrieve(0)]->current_class);
      Locator::getPlayers()
        ->getPlayer(players[received_data.retrieve(0)]->current_class)
        ->setPos(new_pos);
      debug_text.print("Moving player " +
                       std::to_string(received_data.retrieve(0)) +
                       " to room '" + this_room.getName() + "'.");
      break;
    }
    // The active client's action points have changed, update it for us.
    case data_roles::CLIENT_ACTION_POINTS_CHANGED:
    {
      // Update another player's action point count
      if (received_data.retrieve(0) == Locator::getPlayers()->my_player_index)
      {
        break;
      }

      players[received_data.retrieve(0)]->action_points =
        received_data.retrieve(2);

      if (received_data.retrieve(3) != -1)
      {
        board.assignActionPointToIssue(
          players[received_data.retrieve(0)]->current_class,
          received_data.retrieve(3),
          received_data.retrieve(1) - received_data.retrieve(2));
        debug_text.print(
          "Player " + std::to_string(received_data.retrieve(0)) + " assigned " +
          std::to_string(received_data.retrieve(1) -
                         received_data.retrieve(2)) +
          " points to card " + std::to_string(received_data.retrieve(3)) + ".");
      }
      break;
    }
    // If connecting to an in-progress game, the server needs to sync
    // information to us.
    case data_roles::SERVER_SYNCS_CARD_INFO:
    {
      // Sync issue cards.
      int issue_cards[5] = { received_data.retrieve(0),
                             received_data.retrieve(1),
                             received_data.retrieve(2),
                             received_data.retrieve(3),
                             received_data.retrieve(4) };
      board.setActiveIssueCards(issue_cards, true); // should sync ship
                                                    // position here too
      debug_text.print("Sync: updated active issue cards.");
      for (int i = 0; i < 5; i++)
      {
        debug_text.print("Sync: issue card 1: " +
                         std::to_string(received_data.retrieve(i)));
      }
      // Sync my objective card
      board.setActiveObjectiveCard(
        received_data.retrieve(5 + Locator::getPlayers()->my_player_index));
      debug_text.print("Sync: updated my objective card to " +
                       std::to_string(received_data.retrieve(
                         5 + Locator::getPlayers()->my_player_index)) +
                       ".");
      // Sync action points.
      for (int i = 0; i < 4; i++)
      {
        players[i]->action_points = received_data.retrieve(9 + i);
        debug_text.print("Sync: updated player " + std::to_string(i) +
                         "'s points to " +
                         std::to_string(players[i]->action_points) + ".");
      }
      break;
    }
    // client requested item card, if this client is == to the one that
    // requested, add item card to current items.
    case data_roles::CLIENT_REQUESTED_ITEM_CARD:
    {
      debug_text.print("adding item card of type" +
                       std::to_string(received_data.retrieve(1)));
      board.setActiveItemCard(received_data.retrieve(1));
      break;
    }
    case data_roles::SERVER_SYNCS_POSITION_INFO:
    {
      for (int i = 0; i < 4; i++)
      {
        ShipRoom this_room =
          board.getRoom(static_cast<ship_rooms>(received_data.retrieve(i)));
        // Get new movement position and move to it
        Vector2 new_pos = this_room.getPosForPlayer(players[i]->current_class);
        Locator::getPlayers()
          ->getPlayer(players[i]->current_class)
          ->setPos(new_pos);

        // Broadcast out the position of us to keep all clients informed
        if (i == Locator::getPlayers()->my_player_index)
        {
          DataShare new_share =
            DataShare(data_roles::CLIENT_MOVING_PLAYER_TOKEN);
          new_share.add(Locator::getPlayers()->my_player_index);
          new_share.add(static_cast<int>(this_room.getEnum()));
          Locator::getNetworkInterface()->sendData(new_share);
        }

        debug_text.print("Sync: moved player " + std::to_string(i) +
                         " to room '" + this_room.getName() + "'.");
      }
      // Sync ship board progress
      Locator::getPlayers()->current_progress_index = received_data.retrieve(4);
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
      Locator::getPlayers()->current_progress_index = received_data.retrieve(0);
      debug_text.print(
        ". to:" +
        std::to_string(Locator::getPlayers()->current_progress_index));
      break;
    }
    // Anything else is unhandled.
    default:
    {
      debug_text.print("An unhandled data packet was received, of type " +
                         std::to_string(received_data.getType()) + "!",
                       1);
      break;
    }
  }
}

/* Handles key inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  keys.registerEvent(static_cast<const ASGE::KeyEvent*>(data.get()));

  // Force all inputs to popups when visible
  if (issue_card_popup.isVisible() || objective_card_popup.isVisible() ||
      dice_roll_popup.isVisible())
  {
    issue_card_popup.keyHandler(keys);
    objective_card_popup.keyHandler(keys);
    dice_roll_popup.keyHandler(keys);
    return;
  }

  // Game input states
  switch (current_state)
  {
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
        DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
        new_share.add(Locator::getPlayers()->my_player_index);
        Locator::getNetworkInterface()->sendData(new_share);
        current_scene_lock_active = true;
        debug_text.print("Requesting to end my go!!");
      }
      if (keys.keyReleased("Debug Spend AP") &&
          players[Locator::getPlayers()->my_player_index]->is_active)
      {
        // Debug: change my action points to 10.
        int new_ap = 10;
        DataShare new_share =
          DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
        new_share.add(Locator::getPlayers()->my_player_index);
        new_share.add(
          players[Locator::getPlayers()->my_player_index]->action_points);
        new_share.add(new_ap);
        new_share.add(-1);
        Locator::getNetworkInterface()->sendData(new_share);
        players[Locator::getPlayers()->my_player_index]->action_points = new_ap;
        debug_text.print("Debug: changed my action points to 10!");
      }
      if (keys.keyReleased("Debug Buy Item"))
      {
        DataShare new_share = DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
        new_share.add(Locator::getPlayers()->my_player_index);
        Locator::getNetworkInterface()->sendData(new_share);
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
        DataShare new_share =
          DataShare(data_roles::CLIENT_DISCONNECTING_FROM_LOBBY);
        new_share.add(Locator::getPlayers()->my_player_index);
        Locator::getNetworkInterface()->sendData(new_share);
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

  // Only handle clicks if we actually clicked!
  if (click->action != ASGE::E_MOUSE_CLICK)
  {
    return;
  }

  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);

  // Force all inputs to popups when active
  if (issue_card_popup.isVisible() || objective_card_popup.isVisible() ||
      dice_roll_popup.isVisible())
  {
    issue_card_popup.clickHandler();
    objective_card_popup.clickHandler();
    dice_roll_popup.clickHandler();

    // Handle interactions for all active buttons in issue popup when visible
    if (issue_card_popup.isVisible())
    {
      int ap_button_index = 0;
      for (ClickableButton* button : issue_card_popup.getInternalButtons())
      {
        if (button->isActive())
        {
          if (button->clicked())
          {
            int& my_action_points =
              players[Locator::getPlayers()->my_player_index]->action_points;
            int points_to_assign = 1;
            if (my_action_points >= points_to_assign)
            {
              // Assign action point to the selected issue - this currently
              // assigns ONE action point, maybe in future have buttons for
              // varying amounts?
              DataShare new_share =
                DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
              new_share.add(Locator::getPlayers()->my_player_index);
              new_share.add(my_action_points);
              new_share.add(my_action_points - points_to_assign);
              new_share.add(ap_button_index);
              Locator::getNetworkInterface()->sendData(new_share);
              board.assignActionPointToIssue(
                players[Locator::getPlayers()->my_player_index]->current_class,
                ap_button_index,
                points_to_assign);
              my_action_points -= points_to_assign;
              debug_text.print("Assigned action points! My total is now " +
                               std::to_string(my_action_points) + ".");
            }
            else
            {
              // BEEP BOOP WE DON'T HAVE ENOUGH ACTION POINTS, some kind of UI
              // prompt here would be nice!
              debug_text.print("COULD NOT ASSIGN ACTION POINTS! WE HAVE " +
                               std::to_string(my_action_points) + ".");
            }
          }
          ap_button_index++;
        }
      }
    }
    return;
  }

  // Game input states
  switch (current_state)
  {
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
          DataShare new_share =
            DataShare(data_roles::CLIENT_MOVING_PLAYER_TOKEN);
          new_share.add(Locator::getPlayers()->my_player_index);
          new_share.add(static_cast<int>(this_room.getEnum()));
          Locator::getNetworkInterface()->sendData(new_share);
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->setPos(new_pos);
          debug_text.print("Moving my player token to room '" +
                           this_room.getName() + "'.");
        }

        // Clicked end turn button
        if (end_turn_btn.clicked())
        {
          DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
          new_share.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share);
          current_scene_lock_active = true;
          debug_text.print("Requesting to end my go!!");
        }
      }

      /* WHEN CLIENT IS ACTIVE/INACTIVE */
      // Clicked on an objective card
      if (board.isHoveringOverObjectiveCard(mouse_pos))
      {
        objective_card_popup.clearAllReferencedSprites();
        objective_card_popup.referenceSprite(
          *game_sprites.popup_card_shadows[0]);
        objective_card_popup.referenceSprite(
          *board.getClickedObjectiveCard(mouse_pos)->getSprite());
        objective_card_popup.show();
      }
      // Clicked on an issue card
      if (board.isHoveringOverIssueCard(mouse_pos))
      {
        issue_card_popup.clearAllReferencedSprites();
        issue_card_popup.referenceSprite(
          *game_sprites.popup_card_shadows[board.activeIssuesCount()]);
        for (IssueCard& issue_card : board.getIssueCards())
        {
          issue_card_popup.referenceSprite(*issue_card.getSprite());
        }
        // If we're the active player, show the opportunity to assign action
        // points to each card
        if (players[Locator::getPlayers()->my_player_index]->is_active)
        {
          int button_index = 0;
          for (ClickableButton* button : issue_card_popup.getInternalButtons())
          {
            if (button_index == board.activeIssuesCount())
            {
              break;
            }
            button->setActive(true);
            button_index++;
          }
        }
        issue_card_popup.show();
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
  /* POPUPS */

  // Update popups
  issue_card_popup.update(game_time);
  objective_card_popup.update(game_time);
  dice_roll_popup.update(game_time);

  // Update cards if required and show popup if needed
  if (board.updateActiveIssueCards())
  {
    // Hide all old popups
    issue_card_popup.hide();
    objective_card_popup.hide();
    dice_roll_popup.hide();

    // Show issue card popup
    issue_card_popup.clearAllReferencedSprites();
    issue_card_popup.referenceSprite(
      *game_sprites.popup_card_shadows[board.activeIssuesCount()]);
    for (IssueCard& issue_card : board.getIssueCards())
    {
      issue_card_popup.referenceSprite(*issue_card.getSprite());
    }

    // Show/hide all card overlays as required (card overlays match the card
    // size, so we can filter them this way)
    int card_overlay_index = 0;
    for (ScaledSprite* sprite : issue_card_popup.getInternalSprites())
    {
      if (sprite->getBoundingBox().width == card_offsets.issue_popup_size.x &&
          sprite->getBoundingBox().height == card_offsets.issue_popup_size.y)
      {
        if (card_overlay_index < board.activeIssuesCount())
        {
          sprite->show();
        }
        else
        {
          sprite->hide();
        }
        card_overlay_index++;
      }
    }

    // Show popup
    issue_card_popup.showForTime(5);
    is_new_turn = true;
    got_new_obj_this_turn = false;
  }
  if (board.updateActiveObjectiveCard())
  {
    got_new_obj_card = true;
    got_new_obj_this_turn = true;
    debug_text.print("got_new_obj_card");
  }

  // Show objective popup if needed
  if (is_new_turn && got_new_obj_card && !issue_card_popup.isVisible())
  {
    objective_card_popup.clearAllReferencedSprites();
    objective_card_popup.referenceSprite(*game_sprites.popup_card_shadows[0]);
    objective_card_popup.referenceSprite(
      *board.getObjectiveCard()->getSprite());
    objective_card_popup.showForTime(5);
    got_new_obj_card = false;
  }

  // Show dice roll popup if a new active turn
  if (is_new_turn &&
      players[Locator::getPlayers()->my_player_index]->is_active &&
      !issue_card_popup.isVisible() && !objective_card_popup.isVisible())
  {
    dice_roll_popup.show();
    is_new_turn = false;
  }

  /* STATE-SPECIFIC CURSOR */

  // While our cursor hover state is handled by individual buttons, more generic
  // game stuff like the card hovering needs to be handled here. By default, the
  // cursor is set to inactive at the start of every update call, this will then
  // override that to true if needed.

  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);

  if (!objective_card_popup.isVisible() && !issue_card_popup.isVisible() &&
      !dice_roll_popup.isVisible())
  {
    // Update to hover cursor for cards
    if (board.isHoveringOverIssueCard(mouse_pos) ||
        board.isHoveringOverObjectiveCard(mouse_pos))
    {
      Locator::getCursor()->setCursorActive(true);
    }

    // If we're the active player, aren't syncing, and don't already have an
    // active player, check if we're hovering over a room
    if (players[Locator::getPlayers()->my_player_index]->is_active &&
        !current_scene_lock_active && board.isHoveringOverRoom(mouse_pos))
    {
      Locator::getCursor()->setCursorActive(true);
    }
  }

  /* MISC */

  // End turn button is only active when we are
  end_turn_btn.setActive(
    players[Locator::getPlayers()->my_player_index]->is_active);
  if (objective_card_popup.isVisible() || issue_card_popup.isVisible() ||
      dice_roll_popup.isVisible())
  {
    end_turn_btn.setActive(false); // inactive when popups are over us
  }
  end_turn_btn.update();

  return next_scene;
}

/* Render function */
void GameScene::render()
{
  switch (current_state)
  {
    case game_state::PLAYING:
    {
      // Board and background
      renderer->renderSprite(*game_sprites.background->getSprite());
      board.render(objective_card_popup.isVisible(),
                   issue_card_popup.isVisible());

      float active_marker_pos = -180.0f;
      float my_marker_pos = -180.f;
      for (int i = 0; i < 4; i++)
      {
        float this_pos = static_cast<float>(180 * i);

        // Player tab - maybe only render this if they're in-game, as players
        // cued up in the lobby still show this otherwise
        Locator::getPlayers()
          ->getPlayer(players[i]->current_class)
          ->getGameTabSprite()
          ->yPos(this_pos);
        renderer->renderSprite(*Locator::getPlayers()
                                  ->getPlayer(players[i]->current_class)
                                  ->getGameTabSprite()
                                  ->getSprite());

        // draw score if player is connected
        if (players[i]->current_class != player_classes::UNASSIGNED)
        {
          renderer->renderText(std::to_string(players[i]->action_points),
                               225,
                               static_cast<int>(this_pos + 100),
                               1,
                               ASGE::COLOURS::WHITE);
        }

        // log position for active player marker and "you" marker
        if (players[i]->is_active)
        {
          active_marker_pos = this_pos;
        }
        if (i == Locator::getPlayers()->my_player_index)
        {
          my_marker_pos = this_pos;
        }
      }

      // Activity markers
      renderer->renderSprite(*game_sprites.inactive_player_marker->getSprite());
      game_sprites.active_player_marker->yPos(active_marker_pos);
      renderer->renderSprite(*game_sprites.active_player_marker->getSprite());

      // You marker
      game_sprites.your_player_marker->yPos(my_marker_pos);
      renderer->renderSprite(*game_sprites.your_player_marker->getSprite());

      // Progress meters
      renderer->renderSprite(*game_sprites.progress_meter->getSprite());
      game_sprites.progress_marker->yPos(static_cast<float>(
        ((Locator::getPlayers()->current_progress_index + 3.5) * 30)));
      renderer->renderSprite(*game_sprites.progress_marker->getSprite());

      // End-turn button
      end_turn_btn.render();

      break;
    }
    case game_state::LOCAL_PAUSE:
    {
      pause_menu.render();
      break;
    }
  }

  // Render popups if needed
  issue_card_popup.render();
  objective_card_popup.render();
  dice_roll_popup.render();

  // Show the assigned action points per card if visible
  if (issue_card_popup.isVisible())
  {
    issue_card_popup.clearAllRenderText();
    for (IssueCard& card : board.getIssueCards())
    {
      issue_card_popup.renderTextAtPosition(
        std::to_string(card.getAssignedPoints(player_classes::MEDIC)),
        card.getPosition() + Vector2(80 * card.getSprite()->getScalar(),
                                     290 * card.getSprite()->getScalar()));
      issue_card_popup.renderTextAtPosition(
        std::to_string(card.getAssignedPoints(player_classes::COMMUNICATIONS)),
        card.getPosition() + Vector2(183 * card.getSprite()->getScalar(),
                                     290 * card.getSprite()->getScalar()));
      issue_card_popup.renderTextAtPosition(
        std::to_string(card.getAssignedPoints(player_classes::PILOT)),
        card.getPosition() + Vector2(183 * card.getSprite()->getScalar(),
                                     329 * card.getSprite()->getScalar()));
      issue_card_popup.renderTextAtPosition(
        std::to_string(card.getAssignedPoints(player_classes::ENGINEER)),
        card.getPosition() + Vector2(80 * card.getSprite()->getScalar(),
                                     329 * card.getSprite()->getScalar()));
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
    renderer->renderText(
      "IS_ACTIVE: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->is_active),
      10,
      30,
      0.5,
      ASGE::COLOURS::WHITE);
    renderer->renderText(
      "CURRENT_CLASS: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->current_class),
      10,
      50,
      0.5,
      ASGE::COLOURS::WHITE);
    renderer->renderText(
      "HAS_CONNECTED: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->has_connected),
      10,
      70,
      0.5,
      ASGE::COLOURS::WHITE);
    renderer->renderText(
      "IS_READY: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->is_ready),
      10,
      90,
      0.5,
      ASGE::COLOURS::WHITE);
    renderer->renderText(
      "ACTION_POINTS: " +
        std::to_string(
          players[Locator::getPlayers()->my_player_index]->action_points),
      10,
      110,
      0.5,
      ASGE::COLOURS::WHITE);
    renderer->renderText(
      "PRESS M TO FINISH TURN", 10, 130, 0.5, ASGE::COLOURS::WHITE);
    renderer->renderText(
      "PRESS L TO DEBUG TEST ACTION POINTS", 10, 150, 0.5, ASGE::COLOURS::WHITE);
  }
}