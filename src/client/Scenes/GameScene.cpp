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
  ui_manager.popups().createPopup(ui_popups::ITEM_POPUP);
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
  ui_manager.createSprite(ui_sprites::OTHER_PLAYER_DISCONNECTED_OVERLAY, "UI/INGAME_UI/disconnect_overlay.png");
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
  ui_manager.popups()
    .getPopup(ui_popups::ITEM_POPUP)
    ->createSprite("UI/INGAME_UI/item_bg.png");

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
  ui_manager
    .createButton(ui_buttons::OBJECTIVE_BTN,
                  "UI/INGAME_UI/use_objective_btn.png")
    ->setPos(Vector2(847, 551));

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