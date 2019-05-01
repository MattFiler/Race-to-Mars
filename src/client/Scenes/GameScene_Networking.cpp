#include "GameScene.h"
#include "client/Locator/ServiceLocator.h"
#include "client/NetworkConnection/NetworkConnection.h"
#include "client/Scenes/GameScene.h"
#include "gamelib/NetworkedData/NetworkedData.h"

#include <client/Cards/IssueCard.h>
#include <client/Cards/ItemCard.h>
#include <client/Cards/ObjectiveCard.h>
#include <gamelib/Packet.h>

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
      serverEndsClientTurn(received_data);
      break;
    }
      // The active client has moved their player token, update it on our
      // screen.
    case data_roles::CLIENT_MOVING_PLAYER_TOKEN:
    {
      clientMovesPlayerToken(received_data);
      break;
    }
      // The active client's action points have changed, update it for us.
    case data_roles::CLIENT_ACTION_POINTS_CHANGED:
    {
      clientActionPointsUpdated(received_data);
      break;
    }
      // If connecting to an in-progress game, the server needs to sync
      // information to us.
    case data_roles::SERVER_SYNCS_CARD_INFO:
    {
      serverSyncsCardInfo(received_data);
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
        update_item_card += 1;
      }
      break;
    }
    case data_roles::SERVER_SYNCS_POSITION_INFO:
    {
      serverSyncsPositionInfo(received_data);
      break;
    }
    case data_roles::CLIENT_CHANGE_PROGRESS_INDEX:
    {
      debug_text.print(
        "Changing current_progress_index from:" +
        std::to_string(Locator::getPlayers()->current_progress_index));
      Locator::getPlayers()->current_progress_index = received_data.retrieve(0);
      debug_text.print(
        ". to:" +
        std::to_string(Locator::getPlayers()->current_progress_index));
      break;
    }
    case data_roles::CLIENT_SOLVED_ISSUE_CARD:
    {
      debug_text.print("Resyncing issue cards client side... ");
      int sync_issues[5] = { received_data.retrieve(0),
                             received_data.retrieve(1),
                             received_data.retrieve(2),
                             received_data.retrieve(3),
                             received_data.retrieve(4) };
      board.syncIssueCards(sync_issues);
      break;
    }
      // A client has completed an objective card or ended turn on obj spot.
    case data_roles::CLIENT_REQUESTS_OBJ_CARD:
    {
      debug_text.print("Received new obj after completing one of type:" +
                       std::to_string(received_data.retrieve(1)));
      board.setActiveObjectiveCard(received_data.retrieve(1));
      break;
    }
      // A client has used an obj card to get free ship movement for 1 turn.
    case data_roles::CLIENT_FREE_MOVEMENT:
    {
      debug_text.print("Free ship movement for 1 turn.");
      free_player_movement = static_cast<bool>(received_data.retrieve(0));
      break;
    }
      // Handle Chat Messages.
    case data_roles::CHAT_MSG:
    {
      debug_text.print("Storing sent chat msg:" + received_data.getMsg());
      received_chat_msg = received_data.getMsg();
      new_chat_msg = true;
      if (!entering_msg)
      {
        unread_msgs = true;
      }
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

/* Server ends the client's turn */
void GameScene::serverEndsClientTurn(DataShare& received_data)
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
    board.setActiveIssueCards(active_issue_cards,
                              static_cast<bool>(received_data.retrieve(12)));
    board.checkissueSolved();
    free_player_movement = false;
    used_item_this_turn = false;
    if (board.getIssueCards().size() >= 5)
    {
      lost_game = true;
    }
    else if (Locator::getPlayers()->current_progress_index >= 15)
    {
      won_game = true;
    }
    if (received_data.retrieve(13) == Locator::getPlayers()->my_player_index)
    {
      Locator::getPlayers()
        ->getPlayer(
          players[Locator::getPlayers()->my_player_index]->current_class)
        ->setChasingChicken(true);
      got_chicken_card = true;
    }
  }
  // Pull a new objective card if required.
  if (Locator::getPlayers()->current_progress_index % 2 == 0 &&
      Locator::getPlayers()->current_progress_index != 0 &&
      !got_new_obj_this_turn)
  {
    board.setActiveObjectiveCard(
      received_data.retrieve(8 + Locator::getPlayers()->my_player_index));
    debug_text.print("Adding obj card for client " +
                     std::to_string(Locator::getPlayers()->my_player_index) +
                     "of type: " +
                     std::to_string(received_data.retrieve(
                       8 + Locator::getPlayers()->my_player_index)));
  }
  // End the scene lock.
  current_scene_lock_active = false;
  debug_text.print("The server ended the current go, and passed "
                   "active-ness to client " +
                   std::to_string(received_data.retrieve(1)) + ".");
  Locator::getAudio()->play(turn_ends_sfx);
}

/* Client moves their player token */
void GameScene::clientMovesPlayerToken(DataShare& received_data)
{
  Locator::getAudio()->play(move_counter_sfx);

  if (received_data.retrieve(0) == Locator::getPlayers()->my_player_index)
  {
    return;
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
                   std::to_string(received_data.retrieve(0)) + " to room '" +
                   this_room.getName() + "'.");
}

/* A client's action points updated */
void GameScene::clientActionPointsUpdated(DataShare& received_data)
{
  // Update another player's action point count
  if (received_data.retrieve(0) == Locator::getPlayers()->my_player_index)
  {
    return;
  }

  players[received_data.retrieve(0)]->action_points = received_data.retrieve(2);

  if (received_data.retrieve(3) != -1)
  {
    board.assignActionPointToIssue(
      players[received_data.retrieve(0)]->current_class,
      received_data.retrieve(3),
      received_data.retrieve(1) - received_data.retrieve(2));
    debug_text.print(
      "Player " + std::to_string(received_data.retrieve(0)) + " assigned " +
      std::to_string(received_data.retrieve(1) - received_data.retrieve(2)) +
      " points to card " + std::to_string(received_data.retrieve(3)) + ".");
  }
}

/* Server syncs card info */
void GameScene::serverSyncsCardInfo(DataShare& received_data)
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
}

/* Server syncs player position info */
void GameScene::serverSyncsPositionInfo(DataShare& received_data)
{
  for (int i = 0; i < 4; i++)
  {
    ShipRoom this_room =
      board.getRoom(static_cast<ship_rooms>(received_data.retrieve(i)));
    // Get new movement position and move to it
    Vector2 new_pos = this_room.getPosForPlayer(players[i]->current_class);
    Locator::getPlayers()->getPlayer(players[i]->current_class)->setPos(new_pos);
    players[i]->room = this_room.getEnum();

    // Broadcast out the position of us to keep all clients informed
    if (i == Locator::getPlayers()->my_player_index)
    {
      DataShare new_share = DataShare(data_roles::CLIENT_MOVING_PLAYER_TOKEN);
      new_share.add(Locator::getPlayers()->my_player_index);
      new_share.add(static_cast<int>(this_room.getEnum()));
      Locator::getNetworkInterface()->sendData(new_share);
    }

    debug_text.print("Sync: moved player " + std::to_string(i) + " to room '" +
                     this_room.getName() + "'.");
  }
  // Sync ship board progress.
  Locator::getPlayers()->current_progress_index = received_data.retrieve(4);
  debug_text.print(
    "Sync: moved ship to position " +
    std::to_string(Locator::getPlayers()->current_progress_index));
}
