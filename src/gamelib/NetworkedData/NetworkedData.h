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
  /* ^ The client wants to join a lobby. */

  SERVER_GIVES_LOBBY_INFO,
  /* ^ The server is sending lobby info for the client that just joined.
   *   [0] = number of connected clients
   *   [1] = class of client 1 (-1 means disconnect)
   *   [2] = class of client 2 (-1 means disconnect)
   *   [3] = class of client 3 (-1 means disconnect)
   *   [4] = class of client 4 (-1 means disconnect)
   *   [5] = client 1 ready (0=no,1=yes)
   *   [6] = client 2 ready (0=no,1=yes)
   *   [7] = client 3 ready (0=no,1=yes)
   *   [8] = client 4 ready (0=no,1=yes)
   *   [9] = the client's index (-1 = something broke)
   */

  CLIENT_CONNECTED_TO_LOBBY,
  /* ^ A client has connected to the lobby, let all clients know its data.
   *   [0] = the client index
   *   [1] = if the client is ready
   *   [2] = the client's current class
   */

  CLIENT_DISCONNECTING_FROM_LOBBY,
  /* ^ A client has disconnected from the lobby, let everyone know to forget
   * them.
   *   [0] = the client index
   */

  CLIENT_CHANGED_LOBBY_READY_STATE,
  /* ^ A client has changed their ready state in the lobby.
   *   [0] = the ready state (0=unready, 1=ready)
   *   [1] = the client index
   *   [2] = the lobby ID
   */

  SERVER_STARTS_GAME,
  /* ^ All clients in the lobby are ready, signal to start the game.
   *   [0] = the client to start playing first
   *   [1] = are we joining in-progress (1=yes,0=no)
   */

  CLIENT_WANTS_TO_END_TURN,
  /* ^ The active client wants the server to end their go.
   *   [0] = the client index which is trying to end
   */

  SERVER_ENDED_CLIENT_TURN,
  /* ^ Active client turn ended, re-sync game data (may be new if "full
   * rotation"). [0] = the client that finished [1] = the next client to play
   *   [2] = the position of the ship on the board (may not change)
   *   [3] = issue card in slot 1 (may not change)
   *   [4] = issue card in slot 2 (may not change)
   *   [5] = issue card in slot 3 (may not change)
   *   [6] = issue card in slot 4 (may not change)
   *   [7] = issue card in slot 5 (may not change)
   *   [8] = objective card for player 1
   *   [9] = objective card for player 2
   *   [10] = objective card for player 3
   *   [11] = objective card for player 4
   *   [12] = was this a new "full rotation" (1=yes,0=no)
   */

  CLIENT_MOVING_PLAYER_TOKEN,
  /* ^ The active client moved their player token.
   *   [0] = the client index
   *   [1] = the room index
   */

  CLIENT_ACTION_POINTS_CHANGED,
  /* ^ The active client spent or gained action points.
   *   [0] = the client index
   *   [1] = the old action point count
   *   [2] = the new action point count
   *   [3] = the card ID they were assigned to (-1 = no card)
   */

  CLIENT_REQUESTS_SYNC,
  /* ^ The newly joined client wants a data sync for joining an in-progress
     game. */

  SERVER_SYNCS_CARD_INFO,
  /* ^ The server is sending card and score data to a reconnecting player.
   *   [0] = issue card in slot 1
   *   [1] = issue card in slot 2
   *   [2] = issue card in slot 3
   *   [3] = issue card in slot 4
   *   [4] = issue card in slot 5
   *   [5] = objective card for player 1
   *   [6] = objective card for player 2
   *   [7] = objective card for player 3
   *   [8] = objective card for player 4
   *   [9] = the action points for player 1
   *   [10] = the action points for player 2
   *   [11] = the action points for player 3
   *   [12] = the action points for player 4
   */

  SERVER_SYNCS_POSITION_INFO,
  /* ^ The server is sending position info to a reconnecting player.
   *   [0] = the room of client 1
   *   [1] = the room of client 2
   *   [2] = the room of client 3
   *   [3] = the room of client 4
   *   [4] = the ship's position
   */

  CLIENT_CHANGE_PROGRESS_INDEX,
  /* ^ The client or server is sending the new progress index to the server and
   * to the rest of the clients.
   *   [0] = the new current progress index.
   */

  CLIENT_REQUESTED_ITEM_CARD
  /* ^ The client has requested an item card
   *   [0] = client ID that pulled the item.
   *   [1] = item card ID.
   */
};

struct NetworkedData
{
  data_roles role = data_roles::NO_ROLE;
  int content[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
};

#endif // PROJECT_NETWORKEDDATA_H
