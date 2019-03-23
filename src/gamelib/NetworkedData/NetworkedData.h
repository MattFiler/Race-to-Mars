#ifndef PROJECT_NETWORKEDDATA_H
#define PROJECT_NETWORKEDDATA_H

/*
 *
 * This class forms the foundation of networking within the game.
 *
 * A NetworkedData struct is sent around the clients via the server containing a
 * ROLE and CONTENT (up to 10x). The content is an integer array, stored in
 * "content" (1-10), and the role is an enum.
 *
 * The role describes the purpose of the integers - E.G. number of action points
 * assigned, etc. Not every struct role requires all 10 integers, so they are
 * initialised as zero (think of this as null).
 *
 * The role must be checked when receiving a packet of NetworkedData to work out
 * its purpose and how many integers to check the value of.
 *
 * In the enum "roles" each role is described with its integer
 * usage - refer to this when sending/receiving! This is a simple solution to
 * the complex datatype networking issue.
 *
 * The only potential issue with this is that all game logic is handled client
 * side and passed out to each client through the server, giving the opportunity
 * for cheating. Potentially some kind of data validator could be implemented
 * in the server to get around this?
 *
 */

enum data_roles
{
  NO_ROLE,
  /* ^ This is the default role and should never be sent across the network. */

  CLIENT_REQUESTS_TO_JOIN_LOBBY,
  /* ^ The client needs to know lobby info - pls send. */

  SERVER_GIVES_LOBBY_INFO,
  /* ^ The server is kindly sending the lobby info.
   *   [0] = number of connected clients
   *   [1] = class of player 1 (-1 means disconnect)
   *   [2] = class of player 2 (-1 means disconnect)
   *   [3] = class of player 3 (-1 means disconnect)
   *   [4] = class of player 4 (-1 means disconnect)
   *   [5] = player 1 ready (0=no,1=yes)
   *   [6] = player 2 ready (0=no,1=yes)
   *   [7] = player 3 ready (0=no,1=yes)
   *   [8] = player 4 ready (0=no,1=yes)
   *   [9] = the client's player index (-1 = something broke)
   */

  PLAYER_CONNECTED_TO_LOBBY,
  /* ^ A client has connected to the lobby, let all clients know its data.
   *   [0] = the player index
   *   [1] = if the player is ready
   *   [2] = the player's current class
   */

  CLIENT_DISCONNECTING_FROM_LOBBY,
  /* ^ A client has disconnected from the lobby, let everyone know to forget
   * them.
   *   [0] = the player index
   */

  PLAYER_CHANGED_LOBBY_READY_STATE,
  /* ^ A client has changed their ready state in the lobby.
   *   [0] = the ready state (0=unready, 1=ready)
   *   [1] = the player index
   *   [2] = the lobby ID
   */

  SERVER_STARTS_GAME,
  /* ^ All clients in the lobby are ready, signal to start the game. */

  PLAYER_MOVED_WITHIN_SHIP,
  /* ^ The active player has moved within the ship.
   *   [0] = player's new room index
   *   [1] = player's previous room index
   *   [2] = player's action points after move
   *   [3] = player's index
   */

  PLAYER_ASSIGNED_ACTION_POINTS,
  /* ^ The active player has assigned action points to an issue.
   *   [0] = the card points were assigned to
   *   [1] = the number of action points assigned
   *   [2] = if the card was completed (0/1)
   *   [3] = the player's new action point total
   *   [4] = the player's index
   */

  PLAYER_ENDED_TURN,
  /* ^ The active player ended their turn.
   *   [0] = the player's ID that just ended their go
   *   [1] = the next player (if back the beginning, ship needs to increment)
   */

  PLAYER_DREW_ITEM,
  /* ^ The active player drew an item from the supply room.
   *   [0] = the player's ID that drew the item
   *   [1] = the ID of the item card they acquired
   *   [2] = their remaining action points
   */

  NEW_ISSUE_CARD_ADDED,
  /* ^ A new issue card has been added to the board.
   *   [0] = the id of the new issue card
   *   [1] = the id of the new issue card (optional - if zero, data is blank)
   *   [2] = the id of the new issue card (optional - if zero, data is blank)
   *   [3] = the id of the new issue card (optional - if zero, data is blank)
   *   [4] = the id of the new issue card (optional - if zero, data is blank)
   */
};

struct NetworkedData
{
  data_roles role = data_roles::NO_ROLE;
  int content[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};

#endif // PROJECT_NETWORKEDDATA_H
