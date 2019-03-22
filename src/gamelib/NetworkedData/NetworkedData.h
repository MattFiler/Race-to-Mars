#ifndef PROJECT_NETWORKEDDATA_H
#define PROJECT_NETWORKEDDATA_H

/*
 *
 * This class forms the foundation of networking within the game.
 *
 * A NetworkedData struct is sent around the clients via the server containing a
 * ROLE and CONTENT (up to 5x). The content is an integer array, stored in
 * "content" (1-5), and the role is an enum.
 *
 * The role describes the purpose of the integers - E.G. number of action points
 * assigned, etc. Not every struct role requires all 5 integers, so they are
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

  PLAYER_REQUESTS_LOBBY_CONNECTION,
  /* ^ A client is trying to connect to our lobby.
   *   [0] = N/A
   *   [1] = N/A
   *   [2] = N/A
   *   [3] = N/A
   *   [4] = N/A
   */

  PLAYER_LOBBY_REQUEST_ACCEPTED,
  /* ^ The request to connect to a lobby was accepted - load the client.
   *   [0] = number of connected clients (not including you)
   *   [1] = class of player 1 (-1 means disconnect)
   *   [2] = class of player 2 (-1 means disconnect)
   *   [3] = class of player 3 (-1 means disconnect)
   *   [4] = class of player 4 (-1 means disconnect)
   *   NEED TO SEND READY UP DATA HERE TOO - MIGHT HAVE TO EXTEND DATA ARRAY
   */

  PLAYER_CONNECTED_TO_LOBBY,
  /* ^ A client has connected to the lobby.
   *   [0] = the player ID
   *   [1] = N/A
   *   [2] = N/A
   *   [3] = N/A
   *   [4] = N/A
   */

  PLAYER_DISCONNECTED_FROM_LOBBY,
  /* ^ A client has disconnected from the lobby.
   *   [0] = the player ID
   *   [1] = N/A
   *   [2] = N/A
   *   [3] = N/A
   *   [4] = N/A
   */

  PLAYER_CHANGED_LOBBY_READY_STATE,
  /* ^ A client has changed their ready state in the lobby.
   *   [0] = the ready state (0=unready, 1=ready)
   *   [1] = the player ID
   *   [2] = N/A
   *   [3] = N/A
   *   [4] = N/A
   */

  PLAYER_MOVED_WITHIN_SHIP,
  /* ^ The active player has moved within the ship.
   *   [0] = player's new room index
   *   [1] = player's previous room index
   *   [2] = player's action points after move
   *   [3] = player's ID
   *   [4] = N/A
   */

  PLAYER_ASSIGNED_ACTION_POINTS,
  /* ^ The active player has assigned action points to an issue.
   *   [0] = the card points were assigned to
   *   [1] = the number of action points assigned
   *   [2] = if the card was completed (0/1)
   *   [3] = the player's new action point total
   *   [4] = the player's ID
   */

  PLAYER_ENDED_TURN,
  /* ^ The active player ended their turn.
   *   [0] = the player's ID that just ended their go
   *   [1] = the next player (if back the beginning, ship needs to increment)
   *   [2] = N/A
   *   [3] = N/A
   *   [4] = N/A
   */

  PLAYER_DREW_ITEM,
  /* ^ The active player drew an item from the supply room.
   *   [0] = the player's ID that drew the item
   *   [1] = the ID of the item card they acquired
   *   [2] = their remaining action points
   *   [3] = N/A
   *   [4] = N/A
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
  int content[5] = { 0, 0, 0, 0, 0 };
};

#endif // PROJECT_NETWORKEDDATA_H
