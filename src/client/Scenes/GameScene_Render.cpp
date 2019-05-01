#include "GameScene.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "client/Scenes/GameScene.h"
#include "gamelib/NetworkedData/NetworkedData.h"

#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>
#include <gamelib/Packet.h>

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
        ui_manager.popups().getPopup(ui_popups::ISSUE_POPUP)->isVisible(),
        ui_manager.popups().getPopup(ui_popups::ITEM_POPUP)->isVisible());

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

      // Your marker
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
      ui_manager.getButton(ui_buttons::OBJECTIVE_BTN)->render();

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
      if (card.getActionPointsNeeded() != 0)
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

  // If a player has disconnected from the game, let the other players know
  // we're waiting for them to return.
  if (game_is_paused)
  {
    renderer->renderSprite(
      *ui_manager.getSprite(ui_sprites::OTHER_PLAYER_DISCONNECTED_OVERLAY)
         ->getSprite());
  }

  debugOutput();
}
