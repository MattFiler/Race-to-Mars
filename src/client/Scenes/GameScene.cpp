#include "client/Scenes/GameScene.h"
#include "GameScene.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
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
  Menu* pause_menu = ui_manager.createMenu();
  pause_menu->addMenuSprite("INGAME_UI/pause_bg.jpg");
  pause_menu->addMenuItem("MENU_CONTINUE");
  pause_menu->addMenuItem("MENU_QUIT");

  // Get a reference to the client lobby data array
  for (int i = 0; i < 4; i++)
  {
    players[i] = &Locator::getPlayers()->players[i];
    if (players[i]->is_this_client)
    {
      Locator::getPlayers()->my_player_index = i;
    }
  }

  // Create all required sprites - add in the order to render
  for (int i = 0; i < 6; i++)
  {
    ScaledSprite* new_dice_sprite = ui_manager.createSprite(
      ui_sprites::DICE_ROLL_1 + i,
      "UI/INGAME_UI/dice_" + std::to_string(i + 1) + ".png");
    new_dice_sprite->setPos(Vector2(529, 248));
    new_dice_sprite->hide();
  }
  ui_manager.createSprite(ui_sprites::BACKGROUND, "UI/BOARD/background.jpg");
  ui_manager
    .createSprite(ui_sprites::ACTIVE_PLAYER_MARKER,
                  "UI/INGAME_UI/active_player_tab.png")
    ->yPos(-180.0f);
  ui_manager.createSprite(ui_sprites::YOUR_PLAYER_MARKER,
                          "UI/INGAME_UI/your_tab.png");
  ui_manager.createSprite(ui_sprites::INACTIVE_PLAYER_MARKER,
                          "UI/INGAME_UI/inactive_player_tab_singlesprite.png");
  ui_manager.createSprite(ui_sprites::PROGRESS_METER,
                          "UI/INGAME_UI/progress_bar.png");
  ui_manager
    .createSprite(ui_sprites::PROGRESS_MARKER,
                  "UI/INGAME_UI/progress_marker_padded.png")
    ->yPos(89.0f);
  ui_manager.createSprite(ui_sprites::SYNC_OVERLAY, "UI/INGAME_UI/syncing.png");
  ui_manager.createSprite(ui_sprites::DISCONNECT_OVERLAY,
                          "UI/INGAME_UI/syncing_notext.png");
  for (int i = 0; i < 6; i++)
  {
    ui_manager.createSprite(ui_sprites::POPUP_CARD_SHADOWS_0 + i,
                            "UI/INGAME_UI/cards_" + std::to_string(i) + ".png");
  }

  // Create popups
  ui_manager.popups().createPopup(ui_popups::ISSUE_POPUP);
  ui_manager.popups().createPopup(ui_popups::OBJECTIVE_POPUP);
  ui_manager.popups().createPopup(ui_popups::DICE_ROLL_POPUP);

  // Create popup title sprites
  ui_manager.popups()
    .getPopup(ui_popups::ISSUE_POPUP)
    ->createSprite("UI/INGAME_UI/"
                   "new_issues_bg.png");
  ui_manager.popups()
    .getPopup(ui_popups::OBJECTIVE_POPUP)
    ->createSprite("UI/INGAME_UI/new_obj_bg.png");
  ui_manager.popups()
    .getPopup(ui_popups::DICE_ROLL_POPUP)
    ->createSprite("UI/INGAME_UI/dice_roll_bg.png");

  // Position button for new turn
  ui_manager
    .createButton(ui_buttons::END_TURN_BTN, "UI/INGAME_UI/end_turn_btn.png")
    ->setPos(Vector2(1042, 612));
  ui_manager
    .createButton(ui_buttons::BUY_ITEM_BTN, "UI/INGAME_UI/buy_item_btn.png")
    ->setPos(Vector2(1042, 551));

  // Issue popup card placeholder
  ScaledSprite& card_placeholder = ui_manager.popups()
                                     .getPopup(ui_popups::ISSUE_POPUP)
                                     ->createSprite("UI/CARD_IMAGES/"
                                                    "PLACEHOLDER_5.png");
  card_placeholder.setPos(card_offsets.issue_popup_start);
  card_placeholder.setDims(Vector2(card_offsets.issue_popup_size.x * 5,
                                   card_offsets.issue_popup_size.y));

  for (int i = 0; i < 5; i++)
  {
    // Create buttons for issue card popup
    ClickableButton& new_btn = ui_manager.popups()
                                 .getPopup(ui_popups::ISSUE_POPUP)
                                 ->createButton("UI/"
                                                "INGAM"
                                                "E_UI/"
                                                "assig"
                                                "n_ap_"
                                                "butto"
                                                "n_"
                                                "nosha"
                                                "dow."
                                                "png");
    new_btn.setPos(
      card_offsets.issue_popup_ap_btn_start +
      (card_offsets.issue_popup_ap_btn_offset * static_cast<float>(i)));
    new_btn.setActive(false);

    // Create point overlays for issue card popup
    ScaledSprite& new_sprite = ui_manager.popups()
                                 .getPopup(ui_popups::ISSUE_POPUP)
                                 ->createSprite("UI/"
                                                "INGAM"
                                                "E_UI/"
                                                "score"
                                                "_over"
                                                "lay."
                                                "png");
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
    players[Locator::getPlayers()->my_player_index]->room = this_room.getEnum();
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
      players[received_data.retrieve(0)]->room = this_room.getEnum();
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
      debug_text.print("Client that requested item: " +
                       std::to_string(received_data.retrieve(0)) +
                       ". This client: " +
                       std::to_string(Locator::getPlayers()->my_player_index));
      if (Locator::getPlayers()->my_player_index == received_data.retrieve(0))
      {
        debug_text.print(
          "adding item card of type" +
          std::to_string(received_data.retrieve(1)) +
          "for player: " + std::to_string(received_data.retrieve(0)));
        new_item_card = received_data.retrieve(1);
        update_item_card = true;
      }
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
        players[i]->room = this_room.getEnum();

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
  if (ui_manager.popups().anyAreActive())
  {
    ui_manager.popups().keyHandler(keys);
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
        debug_text.print("Trying to buy item card.");
        if (Locator::getPlayers()
              ->getPlayer(static_cast<player_classes>(
                Locator::getPlayers()->my_player_index))
              ->getHeldItemAmount() < 2)
        {
          auto new_share = DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
          new_share.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share);
        }
      }
      break;
    }
    case game_state::LOCAL_PAUSE:
    {
      if (!ui_manager.getMenu()->itemWasSelected(keys))
      {
        break;
      }
      if (ui_manager.getMenu()->selectedItemWas("MENU_CONTINUE"))
      {
        current_state = game_state::PLAYING;
        debug_text.print("Closing pause menu.");
      }
      else if (ui_manager.getMenu()->selectedItemWas("MENU_QUIT"))
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
  if (ui_manager.popups().anyAreActive())
  {
    ui_manager.popups().clickHandler();

    // Handle interactions for all active buttons in issue popup when visible
    if (ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible())
    {
      int ap_button_index = 0;
      for (ClickableButton* button : ui_manager.popups()
                                       .getPopup(ui_popups::ISSUE_POPUP)
                                       ->getInternalButtons())
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
          players[Locator::getPlayers()->my_player_index]->room =
            this_room.getEnum();
          debug_text.print("Moving my player token to room '" +
                           this_room.getName() + "'.");
        }

        // Clicked end turn button
        if (ui_manager.getButton(ui_buttons::END_TURN_BTN)->clicked())
        {
          DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
          new_share.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share);
          current_scene_lock_active = true;
          debug_text.print("Requesting to end my go!!");

          // Call reset variables on item and issues here
          board.resetCardVariables();
        }

        // Clicked buy item button
        if (ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)->clicked())
        {
          // Should validate score and existing items, etc here!
          DataShare new_share =
            DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
          new_share.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share);
          debug_text.print("I want an item card!!");
        }
      }

      /* WHEN CLIENT IS ACTIVE/INACTIVE */
      // Clicked on an objective card
      if (board.isHoveringOverObjectiveCard(mouse_pos))
      {
        ui_manager.popups()
          .getPopup(ui_popups::OBJECTIVE_POPUP)
          ->clearAllReferencedSprites();
        ui_manager.popups()
          .getPopup(ui_popups::OBJECTIVE_POPUP)
          ->referenceSprite(
            *ui_manager.getSprite(ui_sprites::POPUP_CARD_SHADOWS_0));
        ui_manager.popups()
          .getPopup(ui_popups::OBJECTIVE_POPUP)
          ->referenceSprite(
            *board.getClickedObjectiveCard(mouse_pos)->getSprite());
        ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->show();
      }
      // Clicked on an issue card
      if (board.isHoveringOverIssueCard(mouse_pos))
      {
        ui_manager.popups()
          .getPopup(ui_popups::ISSUE_POPUP)
          ->clearAllReferencedSprites();
        ui_manager.popups()
          .getPopup(ui_popups::ISSUE_POPUP)
          ->referenceSprite(*ui_manager.getSprite(
            ui_sprites::POPUP_CARD_SHADOWS_0 + board.activeIssuesCount()));
        for (IssueCard& issue_card : board.getIssueCards())
        {
          ui_manager.popups()
            .getPopup(ui_popups::ISSUE_POPUP)
            ->referenceSprite(*issue_card.getSprite());
        }
        // If we're the active player, show the opportunity to assign action
        // points to each card
        if (players[Locator::getPlayers()->my_player_index]->is_active)
        {
          int button_index = 0;
          for (ClickableButton* button : ui_manager.popups()
                                           .getPopup(ui_popups::ISSUE_POPUP)
                                           ->getInternalButtons())
          {
            if (button_index == board.activeIssuesCount())
            {
              break;
            }
            button->setActive(true);
            button_index++;
          }
        }
        ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->show();
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

  // Update cards if required and show popup if needed
  if (board.updateActiveIssueCards())
  {
    // Hide all old popups
    ui_manager.popups().hideAll();

    // Setup issue card popup
    ui_manager.popups()
      .getPopup(ui_popups::ISSUE_POPUP)
      ->clearAllReferencedSprites();
    ui_manager.popups()
      .getPopup(ui_popups::ISSUE_POPUP)
      ->referenceSprite(*ui_manager.getSprite(ui_sprites::POPUP_CARD_SHADOWS_0 +
                                              board.activeIssuesCount()));
    for (IssueCard& issue_card : board.getIssueCards())
    {
      ui_manager.popups()
        .getPopup(ui_popups::ISSUE_POPUP)
        ->referenceSprite(*issue_card.getSprite());
    }

    // Show/hide all card overlays as required (card overlays match the card
    // size, so we can filter them this way)
    int card_overlay_index = 0;
    for (ScaledSprite* sprite : ui_manager.popups()
                                  .getPopup(ui_popups::ISSUE_POPUP)
                                  ->getInternalSprites())
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
    ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->showForTime(5);
    is_new_turn = true;
    got_new_obj_this_turn = false;
  }
  if (board.updateActiveObjectiveCard())
  {
    // Objective card updated
    got_new_obj_card = true;
    got_new_obj_this_turn = true;
    debug_text.print("got_new_obj_card");
  }

  if (update_item_card)
  {
    debug_text.print("Updating active item cards.");
    if (board.updateActiveItemCard(new_item_card))
    {
      // show popup here.
    }
    update_item_card = false;
  }
  // Show objective popup if needed
  if (is_new_turn && got_new_obj_card &&
      !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible())
  {
    ui_manager.popups()
      .getPopup(ui_popups::OBJECTIVE_POPUP)
      ->clearAllReferencedSprites();
    ui_manager.popups()
      .getPopup(ui_popups::OBJECTIVE_POPUP)
      ->referenceSprite(
        *ui_manager.getSprite(ui_sprites::POPUP_CARD_SHADOWS_0));
    ui_manager.popups()
      .getPopup(ui_popups::OBJECTIVE_POPUP)
      ->referenceSprite(*board.getObjectiveCard()->getSprite());
    ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->showForTime(5);
    got_new_obj_card = false;
  }

  // Show dice roll popup if a new active turn
  if (is_new_turn &&
      players[Locator::getPlayers()->my_player_index]->is_active &&
      !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->show();

    // Roll the dice & use result
    int dice_roll =
      Locator::getPlayers()
        ->getPlayer(
          players[Locator::getPlayers()->my_player_index]->current_class)
        ->getDiceRoll();
    ;
    DataShare new_share = DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
    new_share.add(Locator::getPlayers()->my_player_index);
    new_share.add(
      players[Locator::getPlayers()->my_player_index]->action_points);
    new_share.add(
      dice_roll +
      players[Locator::getPlayers()->my_player_index]->action_points);
    new_share.add(-1);
    Locator::getNetworkInterface()->sendData(new_share);
    players[Locator::getPlayers()->my_player_index]->action_points += dice_roll;
    debug_text.print("Rolled dice! I got " + std::to_string(dice_roll) + ".");

    // Show result
    ScaledSprite* dice_sprite =
      ui_manager.getSprite(ui_sprites::DICE_ROLL_1 + dice_roll - 1);
    dice_sprite->show();
    ui_manager.popups()
      .getPopup(ui_popups::DICE_ROLL_POPUP)
      ->referenceSprite(*dice_sprite);

    is_new_turn = false;
  }

  /* STATE-SPECIFIC CURSOR */

  // While our cursor hover state is handled by individual buttons, more generic
  // game stuff like the card hovering needs to be handled here. By default, the
  // cursor is set to inactive at the start of every update call, this will then
  // override that to true if needed.

  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);

  if (!ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->isVisible())
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

  // Update UI
  ui_manager.update(game_time);

  // End turn button is only active when we are
  ui_manager.getButton(ui_buttons::END_TURN_BTN)
    ->setActive(players[Locator::getPlayers()->my_player_index]->is_active);
  if (ui_manager.popups().anyAreActive())
  {
    ui_manager.getButton(ui_buttons::END_TURN_BTN)->setActive(false); // inactive
                                                                      // when
                                                                      // popups
                                                                      // are
                                                                      // over us
  }

  // Buy item is only active when we are, and in the supply bay
  if (players[Locator::getPlayers()->my_player_index]->is_active &&
      players[Locator::getPlayers()->my_player_index]->room ==
        ship_rooms::SUPPLY_BAY)
  {
    ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)->setActive(true);
  }
  else
  {
    ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)->setActive(false);
  }

  return next_scene;
}

/* Render function */
// This is super messy atm and is due a refactor
void GameScene::render()
{
  switch (current_state)
  {
    case game_state::PLAYING:
    {
      // Board and background
      renderer->renderSprite(
        *ui_manager.getSprite(ui_sprites::BACKGROUND)->getSprite());
      board.render(
        ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->isVisible(),
        ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible());

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
      renderer->renderSprite(
        *ui_manager.getSprite(ui_sprites::INACTIVE_PLAYER_MARKER)->getSprite());
      ui_manager.getSprite(ui_sprites::ACTIVE_PLAYER_MARKER)
        ->yPos(active_marker_pos);
      renderer->renderSprite(
        *ui_manager.getSprite(ui_sprites::ACTIVE_PLAYER_MARKER)->getSprite());

      // You marker
      ui_manager.getSprite(ui_sprites::YOUR_PLAYER_MARKER)->yPos(my_marker_pos);
      renderer->renderSprite(
        *ui_manager.getSprite(ui_sprites::YOUR_PLAYER_MARKER)->getSprite());

      // Progress meters
      renderer->renderSprite(
        *ui_manager.getSprite(ui_sprites::PROGRESS_METER)->getSprite());
      ui_manager.getSprite(ui_sprites::PROGRESS_MARKER)
        ->yPos(static_cast<float>(
          ((Locator::getPlayers()->current_progress_index + 3.5) * 30)));
      renderer->renderSprite(
        *ui_manager.getSprite(ui_sprites::PROGRESS_MARKER)->getSprite());

      // buttons
      ui_manager.getButton(ui_buttons::END_TURN_BTN)->render();
      ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)->render();

      break;
    }
    case game_state::LOCAL_PAUSE:
    {
      ui_manager.getMenu()->render();
      break;
    }
  }

  // Render popups if needed
  ui_manager.popups().render();

  // Show the assigned action points per card if visible
  if (ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->clearAllRenderText();
    for (IssueCard& card : board.getIssueCards())
    {
      ui_manager.popups()
        .getPopup(ui_popups::ISSUE_POPUP)
        ->renderTextAtPosition(
          std::to_string(card.getAssignedPoints(player_classes::MEDIC)),
          card.getPosition() + Vector2(80 * card.getSprite()->getScalar(),
                                       290 * card.getSprite()->getScalar()));
      ui_manager.popups()
        .getPopup(ui_popups::ISSUE_POPUP)
        ->renderTextAtPosition(
          std::to_string(
            card.getAssignedPoints(player_classes::COMMUNICATIONS)),
          card.getPosition() + Vector2(183 * card.getSprite()->getScalar(),
                                       290 * card.getSprite()->getScalar()));
      ui_manager.popups()
        .getPopup(ui_popups::ISSUE_POPUP)
        ->renderTextAtPosition(
          std::to_string(card.getAssignedPoints(player_classes::PILOT)),
          card.getPosition() + Vector2(183 * card.getSprite()->getScalar(),
                                       329 * card.getSprite()->getScalar()));
      ui_manager.popups()
        .getPopup(ui_popups::ISSUE_POPUP)
        ->renderTextAtPosition(
          std::to_string(card.getAssignedPoints(player_classes::ENGINEER)),
          card.getPosition() + Vector2(80 * card.getSprite()->getScalar(),
                                       329 * card.getSprite()->getScalar()));
    }
  }

  // If active, render the "scene lock" overlay (cuts out interaction while
  // syncing)
  if (current_scene_lock_active)
  {
    renderer->renderSprite(
      *ui_manager.getSprite(ui_sprites::SYNC_OVERLAY)->getSprite());
  }
  if (has_disconnected)
  {
    renderer->renderSprite(
      *ui_manager.getSprite(ui_sprites::DISCONNECT_OVERLAY)->getSprite());
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