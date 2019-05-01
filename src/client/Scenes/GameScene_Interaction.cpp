#include "GameScene.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "client/Scenes/GameScene.h"
#include "gamelib/NetworkedData/NetworkedData.h"

#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>
#include <gamelib/Packet.h>

/* Handles Key Inputs */
void GameScene::keyHandler(const ASGE::SharedEventData data)
{
  // Disable input when paused
  if (game_is_paused)
  {
    return;
  }

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
  if (entering_msg && current_state == game_state::PLAYING &&
      event->action == ASGE::KEYS::KEY_PRESSED)
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

/* Input handles for the state of playing */
void GameScene::playingInput()
{
  if (!entering_msg)
  {
    if (keys.keyReleased("Back") && !current_scene_lock_active && !entering_msg)
    {
      current_state = game_state::LOCAL_PAUSE;
      debug_text.print("Opening pause menu.");
    }
#ifdef NDEBUG
  }
#else
    // request new obj card for client.
    if (board.getObjectiveCard() != nullptr &&
        board.checkObjectiveCardComplete(
          getLobbyPlayer(Locator::getPlayers()->my_player_index)->current_class))
    {
      board.addObjCardToInventory();
      DataShare new_share = DataShare(data_roles::CLIENT_REQUESTS_OBJ_CARD);
      new_share.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share);
    }
    // debug end go
    if (keys.keyReleased("End Turn") &&
        players[Locator::getPlayers()->my_player_index]->is_active &&
        !entering_msg)
    {
      DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
      new_share.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share);
      current_scene_lock_active = true;
      debug_text.print("Requesting to end my go!!");
      board.resetCardVariables();
    }
  }
  if (keys.keyReleased("Debug Obj Inventory") && !entering_msg)
  {
    debug_text.print("Creating obj card");
    board.addObjCardToInventory();
  }
  if (keys.keyReleased("Debug Use Objective Action") && !entering_msg)
  {
    debug_text.print("Using OBJ POWER!");
    board.useObjCardDebug();
  }
  if (keys.keyReleased("Debug Spend AP") &&
      players[Locator::getPlayers()->my_player_index]->is_active &&
      !entering_msg)
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
      players[Locator::getPlayers()->my_player_index]->is_active &&
      !entering_msg)
  {
    debug_text.print("Trying to buy item card.");
    if (Locator::getPlayers()
          ->getPlayer(
            static_cast<player_classes>(Locator::getPlayers()->my_player_index))
          ->getHeldItemAmount() < Locator::getPlayers()
                                    ->getPlayer(static_cast<player_classes>(
                                      Locator::getPlayers()->my_player_index))
                                    ->getMaxItems())
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

/* Handles mouse clicks */
void GameScene::clickHandler(const ASGE::SharedEventData data)
{
  // Disable input when paused
  if (game_is_paused)
  {
    return;
  }

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

/* Handle clicks when playing and active */
void GameScene::playingClicksWhenActive(Vector2& mouse_pos)
{
  if (players[Locator::getPlayers()->my_player_index]->is_active)
  {
    // Clicked within a room on the ship
    if (board.isHoveringOverRoom(mouse_pos))
    {
      ShipRoom new_room = board.getClickedRoom(mouse_pos);
      ShipRoom current_room = board.getRoom(static_cast<ship_rooms>(
        players[Locator::getPlayers()->my_player_index]->room));
      bool can_move = current_room.canMoveTo(new_room.getName());
      if (static_cast<int>(new_room.getEnum()) !=
            players[Locator::getPlayers()->my_player_index]->room &&
          can_move)
      {
        bool free_movement =
          ((static_cast<int>(new_room.getEnum()) ==
            Locator::getPlayers()
              ->getPlayer(
                players[Locator::getPlayers()->my_player_index]->current_class)
              ->getStartingRoom()) ||
           free_player_movement);
        if ((players[Locator::getPlayers()->my_player_index]->action_points >
               0 &&
             !free_player_movement) ||
            free_movement)
        {
          // Get new movement position
          Vector2 new_pos = new_room.getPosForPlayer(
            players[Locator::getPlayers()->my_player_index]->current_class);

          // Move, and let everyone know we're moving
          DataShare new_share =
            DataShare(data_roles::CLIENT_MOVING_PLAYER_TOKEN);
          new_share.add(Locator::getPlayers()->my_player_index);
          new_share.add(static_cast<int>(new_room.getEnum()));
          Locator::getNetworkInterface()->sendData(new_share);
          Locator::getPlayers()
            ->getPlayer(
              players[Locator::getPlayers()->my_player_index]->current_class)
            ->setPos(new_pos);
          players[Locator::getPlayers()->my_player_index]->room =
            new_room.getEnum();
          debug_text.print("Moving my player token to room '" +
                           new_room.getName() + "'.");

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
            ->getPlayer(Locator::getPlayers()
                          ->players[Locator::getPlayers()->my_player_index]
                          .current_class)
            ->getHeldItemAmount() <
          Locator::getPlayers()
            ->getPlayer(Locator::getPlayers()
                          ->players[Locator::getPlayers()->my_player_index]
                          .current_class)
            ->getMaxItems())
      {
        if (players[Locator::getPlayers()->my_player_index]->action_points > 0)
        {
          Locator::getPlayers()
            ->getPlayer(Locator::getPlayers()
                          ->players[Locator::getPlayers()->my_player_index]
                          .current_class)
            ->setHeldItems(1);
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
          ui_manager.showInfoPopup("NOT_ENOUGH_AP_ITEM");
          Locator::getAudio()->play(option_disabled_sfx);
        }
      }
      else
      {
        ui_manager.showInfoPopup("MAX_ITEMS");
        Locator::getAudio()->play(option_disabled_sfx);
      }
    }

    // Clicked dice roll button
    if (ui_manager.getButton(ui_buttons::ROLL_DICE_BTN)->clicked())
    {
      // Roll dice
      int dice_roll =
        Locator::getPlayers()
          ->getPlayer(
            players[Locator::getPlayers()->my_player_index]->current_class)
          ->getDiceRoll();

      /* Card-specific dice rolls */
      if (board.getGoodCommunication())
      {
        good_comm_roll = dice_roll;
        board.setGoodCommunication(false);
      }
      else if (board.getBonusMovement())
      {
        // Move ship here
        DataShare new_share =
          DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
        new_share.add(Locator::getPlayers()->current_progress_index +
                      (dice_roll > 1 ? dice_roll / 2 : 0));
        Locator::getNetworkInterface()->sendData(new_share);
        board.setBonusMovement(false);
      }
      else if (board.getPilotBlackHole())
      {
        // If below 4 move ship back 2 spaces. else move forward 2.
        if (dice_roll < 4)
        {
          DataShare new_share =
            DataShare(data_roles::CLIENT_CHANGE_PROGRESS_INDEX);
          new_share.add(Locator::getPlayers()->current_progress_index - 2);
          Locator::getNetworkInterface()->sendData(new_share);
        }
        board.setPilotBlackHole(false);
      }
      else if (!rolled_dice_this_turn && !board.getBonusMovement() &&
               !board.getPilotBlackHole() && !board.getGoodCommunication())
      {
        // Roll dice
        rolled_dice_this_turn = true;
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

      // Show dice roll popup
      ui_manager.popups()
        .getPopup(ui_popups::DICE_ROLL_POPUP)
        ->clearAllReferencedSprites();
      ScaledSprite* dice_sprite =
        ui_manager.getSprite(ui_sprites::DICE_ROLL_1 + dice_roll - 1);
      dice_sprite->show();
      ui_manager.popups()
        .getPopup(ui_popups::DICE_ROLL_POPUP)
        ->referenceSprite(*dice_sprite);
      ui_manager.popups().getPopup(ui_popups::DICE_ROLL_POPUP)->showForTime(3);
      Locator::getAudio()->play(dice_roll_sfx);
    }
  }
}

/* Handle clicks when playing and active or inactive */
void GameScene::playingClicksWhenActiveOrInactive(Vector2& mouse_pos)
{
  // Clicked on Use Objective Card Btn
  if (ui_manager.getButton(ui_buttons::OBJECTIVE_BTN)->clicked())
  {
    debug_text.print("user clicked objective card.");
    board.useObjCardDebug();
  }

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

  // Clicked on an item card
  if (board.isHoveringOverItemCard(mouse_pos))
  {
    ui_manager.popups()
      .getPopup(ui_popups::ITEM_POPUP)
      ->clearAllReferencedSprites();
    ui_manager.popups()
      .getPopup(ui_popups::ITEM_POPUP)
      ->referenceSprite(*ui_manager.getSprite(ui_sprites::POPUP_CARD_SHADOWS_0 +
                                              board.itemCardCount()));
    for (ItemCard& item_card : board.getItemCards())
    {
      ui_manager.popups()
        .getPopup(ui_popups::ITEM_POPUP)
        ->referenceSprite(*item_card.getSprite());
    }
    ui_manager.popups().getPopup(ui_popups::ITEM_POPUP)->show();
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
        button->setActive(false);
        if (button_index == board.activeIssuesCount())
        {
          break;
        }
        if (board.getIssueCards().at(button_index).getActionPointsNeeded() ==
              0 ||
            board.getIssueCards().at(button_index).isSolved())
        {
          button_index++;
          continue; // we don't allow APs to be assigned to zero-point (or
                    // solved) cards - this is handled however for if we ever
                    // change our minds
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
  // visible.
  if (ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible())
  {
    int ap_button_index = -1;
    player_classes issue_card_class = player_classes::UNASSIGNED;
    for (ClickableButton* button : ui_manager.popups()
                                     .getPopup(ui_popups::ISSUE_POPUP)
                                     ->getInternalButtons())
    {
      ap_button_index++;
      if (button->isActive())
      {
        if (button->clicked())
        {
          // If we've already solved an issue, we can't keep adding points!
          if (board.getIssueCards()
                .at(static_cast<size_t>(ap_button_index))
                .isSolved())
          {
            debug_text.print("THIS ISSUE IS ALREADY SOLVED!");
            ui_manager.showInfoPopup("ALREADY_SOLVED");
            Locator::getAudio()->play(option_disabled_sfx);
            continue;
          }

          // We must be in the correct room to assign action points
          if (Locator::getPlayers()
                ->getPlayer(board.getIssueCards()
                              .at(static_cast<size_t>(ap_button_index))
                              .getIssuePlayerType())
                ->getStartingRoom() !=
              static_cast<int>(
                board
                  .getRoom(static_cast<ship_rooms>(
                    players[Locator::getPlayers()->my_player_index]->room))
                  .getEnum()))
          {
            debug_text.print("NOT IN THE CORRECT ROOM TO ASSIGN ACTION "
                             "POINTS!");
            ui_manager.showInfoPopup("WRONG_ROOM");
            Locator::getAudio()->play(option_disabled_sfx);
            continue;
          }

          int& my_action_points =
            players[Locator::getPlayers()->my_player_index]->action_points;
          int points_to_assign = 1;
          issue_card_class = board.getIssueCards()
                               .at(static_cast<size_t>(ap_button_index))
                               .getIssuePlayerType();

          // Check if player has any item cards. If yes add the points to
          // clicked issue
          int current_item_card = 0;
          for (size_t i = 0; i < board.getItemCards().size(); ++i)
          {
            if (board.getItemCards().at(i).getItemPlayerType() ==
                  issue_card_class &&
                !used_item_this_turn)
            {
              points_to_assign = 5;
              my_action_points += 5;
              used_item_this_turn = true;
              debug_text.print("Assign item card points to issue card.");
              Locator::getPlayers()
                ->getPlayer(
                  players[Locator::getPlayers()->my_player_index]->current_class)
                ->useItem();
            }

            if (used_item_this_turn)
            {
              Locator::getPlayers()
                ->getPlayer(Locator::getPlayers()
                              ->players[Locator::getPlayers()->my_player_index]
                              .current_class)
                ->setHeldItems(-1);
              board.eraseItemCard(current_item_card);
            }
            ++current_item_card;
          }

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
            if (!board.assignActionPointToIssue(
                  players[Locator::getPlayers()->my_player_index]->current_class,
                  ap_button_index,
                  points_to_assign))
            {
              DataShare new_share_2 =
                DataShare(data_roles::CLIENT_REQUESTS_SYNC);
              new_share.add(Locator::getPlayers()->my_player_index);
              new_share.add(0);
              Locator::getNetworkInterface()->sendData(new_share_2);
            }
            else
            {
              my_action_points -= points_to_assign;
              debug_text.print("Assigned action points! My total is now " +
                               std::to_string(my_action_points) + ".");

              if (board.getIssueCards().at(ap_button_index).isSolved())
              {
                // We solved the issue - hide UI
                Locator::getAudio()->play(issue_solved_sfx);
                ui_manager.popups()
                  .getPopup(ui_popups::ISSUE_POPUP)
                  ->getInternalButtons()
                  .at(ap_button_index)
                  ->setActive(false);
              }
              else
              {
                // We didn't solve the issue yet
                Locator::getAudio()->play(ap_assigned_sfx);
              }
            }
          }
          else
          {
            // We don't have enough action points
            debug_text.print("COULD NOT ASSIGN ACTION POINTS! WE HAVE " +
                             std::to_string(my_action_points) + ".");
            ui_manager.showInfoPopup("NOT_ENOUGH_AP");
            Locator::getAudio()->play(option_disabled_sfx);
          }
        }
      }
    }
  }
}