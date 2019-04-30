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

  createButtonsAndPopups();

  // Load sounds
  file_handler.loadSound(end_turn_sfx, "end_turn");
  file_handler.loadSound(new_item_sfx, "new_item");
  file_handler.loadSound(dice_roll_sfx, "dice_roll");
  file_handler.loadSound(turn_ends_sfx, "turn_ends");
  file_handler.loadSound(move_counter_sfx, "move_counter");
  file_handler.loadSound(option_disabled_sfx, "option_disabled");
  file_handler.loadSound(ap_assigned_sfx, "assign_ap");

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

/* Create buttons and popups for the scene */
void GameScene::createButtonsAndPopups()
{
  // Create popups
  ui_manager.popups().createPopup(ui_popups::ISSUE_POPUP);
  ui_manager.popups().createPopup(ui_popups::OBJECTIVE_POPUP);
  ui_manager.popups().createPopup(ui_popups::DICE_ROLL_POPUP);
  ui_manager.popups().createPopup(ui_popups::CHICKEN_POPUP);
  ui_manager.popups().createPopup(ui_popups::YOU_WIN_POPUP);
  ui_manager.popups().createPopup(ui_popups::YOU_LOSE_POPUP);

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
  ui_manager.createSprite(ui_sprites::CHAT_BOX, "UI/INGAME_UI/chatbox.png");
  ui_manager.createSprite(ui_sprites::MSG_ALERT, "UI/INGAME_UI/msg_alert.png");
  for (int i = 0; i < 6; i++)
  {
    ui_manager.createSprite(ui_sprites::POPUP_CARD_SHADOWS_0 + i,
                            "UI/INGAME_UI/cards_" + std::to_string(i) + ".png");
  }

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
  ui_manager.popups()
    .getPopup(ui_popups::CHICKEN_POPUP)
    ->createSprite("UI/INGAME_UI/popup_chicken.png");
  ui_manager.popups()
    .getPopup(ui_popups::YOU_WIN_POPUP)
    ->createSprite("UI/INGAME_UI/you_win_popup.png");
  ui_manager.popups()
    .getPopup(ui_popups::YOU_LOSE_POPUP)
    ->createSprite("UI/INGAME_UI/you_lose_popup.png");

  // Position main ui buttons
  ui_manager
    .createButton(ui_buttons::ROLL_DICE_BTN, "UI/INGAME_UI/roll_dice_btn.png")
    ->setPos(Vector2(847, 612));
  ui_manager
    .createButton(ui_buttons::END_TURN_BTN, "UI/INGAME_UI/end_turn_btn.png")
    ->setPos(Vector2(1042, 612));
  ui_manager
    .createButton(ui_buttons::BUY_ITEM_BTN, "UI/INGAME_UI/buy_item_btn.png")
    ->setPos(Vector2(1042, 551));
  ui_manager.createButton(ui_buttons::CHAT_BTN, "UI/INGAME_UI/chat_btn.png")
    ->setPos(Vector2(330, 679));

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

/* Handles Key Inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  auto event = static_cast<const ASGE::KeyEvent*>(data.get());
  chatMessageInput(event);

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
      // Is playing
    case game_state::PLAYING:
    {
      playingInput();
      break;
    }
    // Is paused
    default:
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

/* Handle chat input */
void GameScene::chatMessageInput(const ASGE::KeyEvent* event)
{
  if (entering_msg && current_state == game_state::PLAYING)
  {
    if (event->action == ASGE::KEYS::KEY_PRESSED)
    {
      if (event->key == ASGE::KEYS::KEY_BACKSPACE && my_chat_msg.length() > 0)
      {
        my_chat_msg.resize(my_chat_msg.size() - 1);
      }

      if (event->key == ASGE::KEYS::KEY_ENTER && my_chat_msg.length() > 0)
      {
        my_chat_msg =
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->getFriendlyName() +
          ": " + my_chat_msg;
        // send chat message to server here.
        auto new_share = DataShare(data_roles::CHAT_MSG);
        new_share.addMsg(my_chat_msg);
        Locator::getNetworkInterface()->sendData(new_share);
        my_chat_msg.clear();
      }
      else if (event->key != ASGE::KEYS::KEY_BACKSPACE &&
               my_chat_msg.length() < max_message_size)
      {
        if (event->key >= 65 && event->key <= 90)
        {
          my_chat_msg += static_cast<char>(event->key);
        }
        else if (event->key == 32)
        {
          my_chat_msg += static_cast<char>(event->key);
        }
      }
    }
  }
}

/* Input handles for the state of playing */
void GameScene::playingInput()
{
  if (!entering_msg)
  {
    if (keys.keyReleased("Back") && !current_scene_lock_active)
    {
      current_state = game_state::LOCAL_PAUSE;
      debug_text.print("Opening pause menu.");
    }
#ifndef NDEBUG
    if (keys.keyReleased("End Turn") &&
        players[Locator::getPlayers()->my_player_index]->is_active)
    {
      if (board.getObjectiveCard() != nullptr)
      {
        // request new obj card for client.
        if (board.checkObjectiveCardComplete(
              getLobbyPlayer(Locator::getPlayers()->my_player_index)
                ->current_class))
        {
          board.addObjCardToInventory();
          DataShare new_share = DataShare(data_roles::CLIENT_REQUESTS_OBJ_CARD);
          new_share.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share);
        }
      }
      DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
      new_share.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share);
      current_scene_lock_active = true;
      debug_text.print("Requesting to end my go!!");
      board.resetCardVariables();
    }
    if (keys.keyReleased("Debug Obj Inventory"))
    {
      debug_text.print("Creating obj card");
      board.addObjCardToInventory();
    }
    if (keys.keyReleased("Debug Use Objective Action"))
    {
      debug_text.print("Using OBJ POWER!");
      board.useObjCardDebug();
    }
    if (keys.keyReleased("Debug Spend AP") &&
        players[Locator::getPlayers()->my_player_index]->is_active)
    {
      // Debug: change my action points to 10.
      int new_ap = 10;
      DataShare new_share = DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
      new_share.add(Locator::getPlayers()->my_player_index);
      new_share.add(
        players[Locator::getPlayers()->my_player_index]->action_points);
      new_share.add(new_ap);
      new_share.add(-1);
      Locator::getNetworkInterface()->sendData(new_share);
      players[Locator::getPlayers()->my_player_index]->action_points = new_ap;
      debug_text.print("Debug: changed my action points to 10!");
    }
    if (keys.keyReleased("Debug Buy Item") &&
        players[Locator::getPlayers()->my_player_index]->is_active)
    {
      debug_text.print("Trying to buy item card.");
      if (Locator::getPlayers()
              ->getPlayer(static_cast<player_classes>(
                Locator::getPlayers()->my_player_index))
              ->getHeldItemAmount() <
            Locator::getPlayers()
              ->getPlayer(static_cast<player_classes>(
                Locator::getPlayers()->my_player_index))
              ->getMaxItems() &&
          players[Locator::getPlayers()->my_player_index]->action_points >= 2)
      {
        auto new_share = DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
        new_share.add(Locator::getPlayers()->my_player_index);
        Locator::getNetworkInterface()->sendData(new_share);
        Locator::getPlayers()
          ->getPlayer(
            static_cast<player_classes>(Locator::getPlayers()->my_player_index))
          ->setHeldItems(1);
        // ap changed here
        int& my_action_points =
          players[Locator::getPlayers()->my_player_index]->action_points;
        DataShare new_share_item =
          DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
        new_share_item.add(Locator::getPlayers()->my_player_index);
        new_share_item.add(my_action_points);
      }
    }
#endif
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
    issuePopupClicks();
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

      playingClicksWhenActive(mouse_pos);

      playingClicksWhenActiveOrInactive(mouse_pos);
    }
    default:
    {
      break;
    }
  }
}

/* Handle clicks when playing and active (THIS NEEDS A GIANT FUCKING REFACTOR!)
 */
void GameScene::playingClicksWhenActive(Vector2& mouse_pos)
{
  if (players[Locator::getPlayers()->my_player_index]->is_active)
  {
    // Clicked within a room on the ship
    if (board.isHoveringOverRoom(mouse_pos))
    {
      ShipRoom this_room = board.getClickedRoom(mouse_pos);
      bool free_movement =
        ((static_cast<int>(this_room.getEnum()) ==
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->getStartingRoom()) ||
         free_player_movement);
      if ((players[Locator::getPlayers()->my_player_index]->action_points > 0 &&
           !free_player_movement) ||
          free_movement)
      {
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
        players[Locator::getPlayers()->my_player_index]->room =
          this_room.getEnum();
        debug_text.print("Moving my player token to room '" +
                         this_room.getName() + "'.");

        if (!free_movement)
        {
          debug_text.print("Moving costs 1 AP.");
          int& my_action_points =
            players[Locator::getPlayers()->my_player_index]->action_points;
          DataShare new_share2 =
            DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
          new_share2.add(Locator::getPlayers()->my_player_index);
          new_share2.add(my_action_points);
          if (!free_player_movement &&
              players[Locator::getPlayers()->my_player_index]->action_points >
                0)
          {
            my_action_points -= 1;
          }
          new_share2.add(my_action_points);
          new_share2.add(-1);
          Locator::getNetworkInterface()->sendData(new_share2);
        }
      }
    }

    // Clicked end turn button
    if (ui_manager.getButton(ui_buttons::END_TURN_BTN)->clicked())
    {
      if (board.getObjectiveCard() != nullptr)
      {
        debug_text.print("Checking if OBJ card is complete.");
        // request new obj card for client.
        if (board.checkObjectiveCardComplete(
              getLobbyPlayer(Locator::getPlayers()->my_player_index)
                ->current_class))
        {
          debug_text.print("Objective card complete! Creating new "
                           "one... "
                           "and adding current obj to inventory.");
          board.addObjCardToInventory();
          DataShare new_share = DataShare(data_roles::CLIENT_REQUESTS_OBJ_CARD);
          new_share.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share);
          // add objective card to inventory.
        }
      }
      DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
      new_share.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share);
      current_scene_lock_active = true;
      debug_text.print("Requesting to end my go!!");

      Locator::getAudio()->play(end_turn_sfx);

      // Call reset variables on item and issues here
      board.resetCardVariables();
    }

    // Clicked buy item button
    if (ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)->clicked())
    {
      // Should validate score and existing items, etc here!
      if (Locator::getPlayers()
            ->getPlayer(static_cast<player_classes>(
              Locator::getPlayers()->my_player_index))
            ->getHeldItemAmount() < Locator::getPlayers()
                                      ->getPlayer(static_cast<player_classes>(
                                        Locator::getPlayers()->my_player_index))
                                      ->getMaxItems())
      {
        // TODO: Shouldn't we check to see if we're over the max number of items
        // here? You can currently go forever!
        if (players[Locator::getPlayers()->my_player_index]->action_points >=
            2) // TODO: shouldn't this be 1, not 2?
        {
          DataShare new_share_item =
            DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
          new_share_item.add(Locator::getPlayers()->my_player_index);
          Locator::getNetworkInterface()->sendData(new_share_item);
          debug_text.print("I want an item card!!");

          Locator::getAudio()->play(new_item_sfx);

          // Below is buying item card.
          int& my_action_points =
            players[Locator::getPlayers()->my_player_index]->action_points;
          DataShare new_share =
            DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
          new_share.add(Locator::getPlayers()->my_player_index);
          new_share.add(my_action_points);
          if (!free_player_movement &&
              players[Locator::getPlayers()->my_player_index]->action_points >
                0)
          {
            my_action_points -= 1;
          }
          new_share.add(my_action_points);
          new_share.add(-1);
          Locator::getNetworkInterface()->sendData(new_share);
        }
        else
        {
          Locator::getAudio()->play(option_disabled_sfx);
        }
      }
    }

    // REFACTOR THIS DICE ROLL SCRIPT - SO MUCH IS DUPLICATED!!

    // Clicked dice roll button
    if (ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->clicked())
    {
      if (board.getGoodCommunication())
      {
        good_comm_roll =
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->getDiceRoll();
        board.setGoodCommunication(false);

        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->clearAllReferencedSprites();
        ScaledSprite* dice_sprite =
          ui_manager.getSprite(ui_sprites::DICE_ROLL_1 + good_comm_roll - 1);
        dice_sprite->show();
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->referenceSprite(*dice_sprite);
        ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->show();

        Locator::getAudio()->play(dice_roll_sfx);
      }
      else if (board.getBonusMovement())
      {
        // Roll dice
        int progress_change = 0;
        int dice_roll =
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->getDiceRoll();
        // Move ship here
        if (dice_roll > 1)
        {
          progress_change = dice_roll / 2;
        }
        DataShare new_share =
          DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
        new_share.add(Locator::getPlayers()->current_progress_index +
                      progress_change);
        Locator::getNetworkInterface()->sendData(new_share);

        // Show result
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->clearAllReferencedSprites();
        ScaledSprite* dice_sprite =
          ui_manager.getSprite(ui_sprites::DICE_ROLL_1 + dice_roll - 1);
        dice_sprite->show();
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->referenceSprite(*dice_sprite);
        ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->show();
        Locator::getAudio()->play(dice_roll_sfx);
        board.setBonusMovement(false);
      }
      else if (board.getPilotBlackHole())
      {
        // Roll dice
        int progress_change = 0;
        int dice_roll =
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->getDiceRoll();
        ;
        // If below 4 move ship back 2 spaces. else move forward 2.
        if (dice_roll < 4)
        {
          progress_change = -2;
          DataShare new_share =
            DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
          new_share.add(Locator::getPlayers()->current_progress_index +
                        progress_change);
          Locator::getNetworkInterface()->sendData(new_share);
        }

        // Show result
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->clearAllReferencedSprites();
        ScaledSprite* dice_sprite =
          ui_manager.getSprite(ui_sprites::DICE_ROLL_1 + dice_roll - 1);
        dice_sprite->show();
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->referenceSprite(*dice_sprite);
        ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->show();
        Locator::getAudio()->play(dice_roll_sfx);
        board.setPilotBlackHole(false);
      }
      else if (!rolled_dice_this_turn && !board.getBonusMovement() &&
               !board.getPilotBlackHole() && !board.getGoodCommunication())
      {
        // Roll dice
        rolled_dice_this_turn = true;
        int dice_roll = 0;
        dice_roll =
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->getDiceRoll();

        // Show result
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->clearAllReferencedSprites();
        ScaledSprite* dice_sprite =
          ui_manager.getSprite(ui_sprites::DICE_ROLL_1 + dice_roll - 1);
        dice_sprite->show();
        ui_manager.popups()
          .getPopup(ui_popups::DICE_ROLL_POPUP)
          ->referenceSprite(*dice_sprite);
        ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->show();
        Locator::getAudio()->play(dice_roll_sfx);

        if (good_comm_roll > dice_roll)
        {
          dice_roll = good_comm_roll;
          good_comm_roll = 0;
        }

        DataShare new_share =
          DataShare(data_roles::CLIENT_ACTION_POINTS_CHANGED);
        new_share.add(Locator::getPlayers()->my_player_index);
        new_share.add(
          players[Locator::getPlayers()->my_player_index]->action_points);
        new_share.add(
          dice_roll +
          players[Locator::getPlayers()->my_player_index]->action_points);
        new_share.add(-1);
        Locator::getNetworkInterface()->sendData(new_share);
        players[Locator::getPlayers()->my_player_index]->action_points +=
          dice_roll;
        debug_text.print("Rolled dice! I got " + std::to_string(dice_roll) +
                         ".");
      }
    }
  }
}

/* Handle clicks when playing and active or inactive */
void GameScene::playingClicksWhenActiveOrInactive(Vector2& mouse_pos)
{
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
      ->referenceSprite(*board.getClickedObjectiveCard(mouse_pos)->getSprite());
    ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->show();
  }

  // Clicked on CHAT btn
  if (ui_manager.getButton(ui_buttons::CHAT_BTN)->clicked())
  {
    if (!entering_msg)
    {
      unread_msgs = false;
    }
    entering_msg = !entering_msg;
  }

  // Clicked on an issue card
  if (board.isHoveringOverIssueCard(mouse_pos))
  {
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
    // If we're the active player, show the opportunity to assign action
    // points to each card.
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

/* Handle clicks in the issue popup */
void GameScene::issuePopupClicks()
{
  // Handle interactions for all active buttons in issue popup when
  // visible
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
            Locator::getPlayers()
              ->getPlayer(static_cast<player_classes>(
                Locator::getPlayers()->my_player_index))
              ->setUsedAPThisTurn(true);
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
            Locator::getAudio()->play(ap_assigned_sfx);
          }
          else
          {
            // BEEP BOOP WE DON'T HAVE ENOUGH ACTION POINTS, some kind of
            // UI prompt here would be nice!
            debug_text.print("COULD NOT ASSIGN ACTION POINTS! WE HAVE " +
                             std::to_string(my_action_points) + ".");
            Locator::getAudio()->play(option_disabled_sfx);
          }
        }
        ap_button_index++;
      }
    }
  }
}

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  // Update popups
  updatePopups(game_time);

  // Check for updates to objective cards
  if (board.updateActiveObjectiveCard())
  {
    // Objective card updated
    got_new_obj_card = true;
    got_new_obj_this_turn = true;
    debug_text.print("got_new_obj_card");
  }

  // Check for new chat messages
  if (new_chat_msg)
  {
    debug_text.print("Adding message: " + received_chat_msg + " to ALL_MSGS.");
    if (chat_messages.size() >= max_messages)
    {
      chat_messages.erase(chat_messages.begin());
    }
    chat_messages.emplace_back(received_chat_msg);
    new_chat_msg = false;
  }

  // Replenish item cards for new ones if obj card has been played.
  if (Locator::getPlayers()
        ->getPlayer(
          static_cast<player_classes>(Locator::getPlayers()->my_player_index))
        ->getReplenishItems())
  {
    board.clearItems();
    for (int i = 0;
         i < Locator::getPlayers()
               ->getPlayer(
                 players[Locator::getPlayers()->my_player_index]->current_class)
               ->getHeldItemAmount();
         ++i)
    {
      DataShare new_share_item =
        DataShare(data_roles::CLIENT_REQUESTED_ITEM_CARD);
      new_share_item.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share_item);
    }

    Locator::getPlayers()
      ->getPlayer(
        static_cast<player_classes>(Locator::getPlayers()->my_player_index))
      ->setReplenishItems(false);
  }

  // Check (and perform) item card updates
  if (update_item_card)
  {
    debug_text.print("Updating active item cards.");
    if (board.updateActiveItemCard(new_item_card))
    {
      // show popup here.
    }
    update_item_card = false;
  }

  // Update popups visibility
  updatePopupVisibility(game_time);

  // Update state specific cursor
  updateStateSpecificCursor(game_time);

  // Update UI
  updateButtonStates(game_time);

  return next_scene;
}

/* POPUPS */
void GameScene::updatePopups(const ASGE::GameTime& game_time)
{
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
    // size, so we can filter them this way.)
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
    rolled_dice_this_turn = false;
  }
}

/* Update Popups */
void GameScene::updatePopupVisibility(const ASGE::GameTime& game_time)
{
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
    // ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->show();
    is_new_turn = false;
  }

  // Show chasing chicken popup
  if (Locator::getPlayers()
        ->getPlayer(Locator::getPlayers()
                      ->players[Locator::getPlayers()->my_player_index]
                      .current_class)
        ->getChasingChicken() &&
      got_chicken_card &&
      !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::CHICKEN_POPUP)->showForTime(5);
    got_chicken_card = false;
  }

  // Show win/Lose states
  if (won_game &&
      !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::OBJECTIVE_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::CHICKEN_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::YOU_WIN_POPUP)->showForTime(60);
  }
  else if (lost_game &&
           !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible() &&
           !ui_manager.popups()
              .getPopup(ui_popups::OBJECTIVE_POPUP)
              ->isVisible() &&
           !ui_manager.popups().getPopup(ui_popups::CHICKEN_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::YOU_LOSE_POPUP)->showForTime(60);
  }
}

/* STATE-SPECIFIC CURSOR */
void GameScene::updateStateSpecificCursor(const ASGE::GameTime& game_time)
{
  // While our cursor hover state is handled by individual buttons, more
  // generic game stuff like the card hovering needs to be handled here. By
  // default, the cursor is set to inactive at the start of every update
  // call, this will then override that to true if needed.

  Vector2 mouse_pos = Vector2(Locator::getCursor()->getPosition().x,
                              Locator::getCursor()->getPosition().y);

  if (!ui_manager.popups().anyAreActive())
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
}

/* UI Updates */
void GameScene::updateButtonStates(const ASGE::GameTime& game_time)
{
  // Update UI
  ui_manager.update(game_time);

  // Buy item is only active when we are, and in the supply bay
  ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)
    ->setActive(players[Locator::getPlayers()->my_player_index]->is_active &&
                players[Locator::getPlayers()->my_player_index]->room ==
                  ship_rooms::SUPPLY_BAY);

  // End turn button is only active when we have completed all dice rolls
  ui_manager.getButton(ui_buttons::END_TURN_BTN)
    ->setActive(Locator::getPlayers()
                  ->players[Locator::getPlayers()->my_player_index]
                  .is_active &&
                !ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->isActive());

  if (!rolled_dice_this_turn)
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(true);
  }
  else if (board.getPilotBlackHole())
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(true);
  }
  else if (board.getBonusMovement())
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(true);
  }

  if (rolled_dice_this_turn && !board.getPilotBlackHole() &&
      !board.getBonusMovement())
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(false);
  }

  // Roll dice button is active when useable
  ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)
    ->setActive(players[Locator::getPlayers()->my_player_index]->is_active &&
                !rolled_dice_this_turn);
  if (rolled_dice_this_turn && board.getPilotBlackHole())
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(true);
  }
  else if (rolled_dice_this_turn && board.getBonusMovement())
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(true);
  }

  // All are inactive when popups are over us
  if (ui_manager.popups().anyAreActive())
  {
    ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->setActive(false);
    ui_manager.getButton(ui_buttons::END_TURN_BTN)->setActive(false);
    ui_manager.getButton(ui_buttons::BUY_ITEM_BTN)->setActive(false);
  }
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

        // Player tab - maybe only render this if they're in-game, as
        // players cued up in the lobby still show this otherwise
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
      ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->render();
      ui_manager.getButton(ui_buttons::CHAT_BTN)->render();

      // Render chat on top of everything else.
      if (entering_msg)
      {
        renderer->renderSprite(
          *ui_manager.getSprite(ui_sprites::CHAT_BOX)->getSprite());
        renderer->renderText(my_chat_msg, 420, 705, 0.5f, ASGE::COLOURS::WHITE);
        if (!chat_messages.empty())
        {
          for (size_t i = 0; i < chat_messages.size(); ++i)
          {
            renderer->renderText(chat_messages[i],
                                 420,
                                 static_cast<int>((50) + (i * 20)),
                                 0.5f,
                                 ASGE::COLOURS::WHITE);
          }
        }
      }
      if (unread_msgs)
      {
        renderer->renderSprite(
          *ui_manager.getSprite(ui_sprites::MSG_ALERT)->getSprite());
      }
      break;
    }
    default:
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

  debugOutput();
}

// client debugging
void GameScene::debugOutput()
{
#ifndef NDEBUG
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
    renderer->renderText(
      "PRESS I TO DEBUG TEST ITEMS", 10, 170, 0.5, ASGE::COLOURS::WHITE);
  }
#endif
}