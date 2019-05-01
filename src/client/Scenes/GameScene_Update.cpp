#include "GameScene.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "client/Scenes/GameScene.h"
#include "gamelib/NetworkedData/NetworkedData.h"

#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>
#include <gamelib/Packet.h>

/* Update function */
game_global_scenes GameScene::update(const ASGE::GameTime& game_time)
{
  // Check to see if we should auto-exit
  if (game_over_timer_started)
  {
    game_over_timer += game_time.delta.count() / 1000;
    if (game_over_timer > 10)
    {
      // Game over! Disconnect and return to main menu.
      DataShare new_share =
        DataShare(data_roles::CLIENT_DISCONNECTING_FROM_LOBBY);
      new_share.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share);
      next_scene = game_global_scenes::MAIN_MENU;
      debug_text.print("Returning to main menu and disconnecting from "
                       "lobby.");
    }
  }

  // Time-out after 60 seconds if we don't get another player connect
  if (game_is_paused)
  {
    // Keep track of time, and timeout if needed
    game_pause_timer += game_time.delta.count() / 1000;
    if (game_pause_timer > 60)
    {
      DataShare new_share =
        DataShare(data_roles::CLIENT_DISCONNECTING_FROM_LOBBY);
      new_share.add(Locator::getPlayers()->my_player_index);
      Locator::getNetworkInterface()->sendData(new_share);
      next_scene = game_global_scenes::MAIN_MENU;
      debug_text.print("Returning to main menu and disconnecting from "
                       "lobby.");
    }

    // If we're now back at 4 players, continue the game.
    int connected_count = 0;
    for (int i = 0; i < 4; i++)
    {
      if (players[i]->is_ready)
      {
        connected_count++;
      }
    }
    if (connected_count == 4)
    {
      game_is_paused = false;
    }

    return next_scene;
  }
  game_pause_timer = 0;

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

    debug_text.print(
      "Creating " +
      std::to_string(
        Locator::getPlayers()
          ->getPlayer(
            players[Locator::getPlayers()->my_player_index]->current_class)
          ->getHeldItemAmount()) +
      " new item cards.");
    Locator::getPlayers()
      ->getPlayer(
        static_cast<player_classes>(Locator::getPlayers()->my_player_index))
      ->setReplenishItems(false);
  }

  // End client turn auto if chasing chicken
  if (Locator::getPlayers()
        ->getPlayer(Locator::getPlayers()
                      ->players[Locator::getPlayers()->my_player_index]
                      .current_class)
        ->getChasingChicken() &&
      players[Locator::getPlayers()->my_player_index]->is_active)
  {
    DataShare new_share = DataShare(data_roles::CLIENT_WANTS_TO_END_TURN);
    new_share.add(Locator::getPlayers()->my_player_index);
    Locator::getNetworkInterface()->sendData(new_share);
    current_scene_lock_active = true;
    debug_text.print("Requesting to end my go!!");
    board.resetCardVariables();

    // Setting chicken chase to false.
    Locator::getPlayers()
      ->getPlayer(Locator::getPlayers()
                    ->players[Locator::getPlayers()->my_player_index]
                    .current_class)
      ->setChasingChicken(false);
  }

  // Check (and perform) item card updates
  if (update_item_card != 0)
  {
    debug_text.print("Updating active item cards.");
    if (board.updateActiveItemCard(new_item_card))
    {
      // show popup here.
    }
    update_item_card -= 1;
  }

  // Update popups visibility
  updatePopupVisibility(game_time);

  // Update state specific cursor
  updateStateSpecificCursor(game_time);

  // Update UI
  updateButtonStates(game_time);

  // Resync Issue Cards
  if (!current_scene_lock_active && new_turn)
  {
    board.prepReSync();
    new_turn = false;
    DataShare new_share = DataShare(data_roles::CLIENT_REQUESTS_SYNC);
    new_share.add(Locator::getPlayers()->my_player_index);
  }
  // Fix for objective card popup on rejoin
  if (just_reconnected)
  {
    ui_manager.popups().hideAll();
    just_reconnected = false;
  }

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
    int card_overlay_index = -1;
    for (ScaledSprite* sprite : ui_manager.popups()
                                  .getPopup(ui_popups::ISSUE_POPUP)
                                  ->getInternalSprites())
    {
      if (sprite->getBoundingBox().width == card_offsets.issue_popup_size.x &&
          sprite->getBoundingBox().height == card_offsets.issue_popup_size.y)
      {
        card_overlay_index++;
        sprite->hide();
        if (card_overlay_index >= board.activeIssuesCount())
        {
          continue;
        }
        if (board.getIssueCards()
              .at(card_overlay_index)
              .getActionPointsNeeded() == 0)
        {
          continue;
        }
        sprite->show();
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
      !ui_manager.popups().getPopup(ui_popups::CHICKEN_POPUP)->isVisible() &&
      !ui_manager.popups().getPopup(ui_popups::YOU_WIN_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::YOU_WIN_POPUP)->showForTime(10);
    game_over_timer_started = true;
  }
  else if (lost_game &&
           !ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible() &&
           !ui_manager.popups()
              .getPopup(ui_popups::OBJECTIVE_POPUP)
              ->isVisible() &&
           !ui_manager.popups().getPopup(ui_popups::CHICKEN_POPUP)->isVisible() &&
           !ui_manager.popups().getPopup(ui_popups::YOU_LOSE_POPUP)->isVisible())
  {
    ui_manager.popups().getPopup(ui_popups::YOU_LOSE_POPUP)->showForTime(10);
    game_over_timer_started = true;
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
        board.isHoveringOverObjectiveCard(mouse_pos) ||
        board.isHoveringOverItemCard(mouse_pos))
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

  ui_manager.getButton(ui_buttons::OBJECTIVE_BTN)
    ->setActive(board.getObjectiveInventory().size() > static_cast<size_t>(0) &&
                players[Locator::getPlayers()->my_player_index]->is_active);

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
    ui_manager.getButton(ui_buttons::OBJECTIVE_BTN)->setActive(false);
  }
}