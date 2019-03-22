#ifndef PROJECT_NETWORKEDDATA_H
#define PROJECT_NETWORKEDDATA_H

/*
 *
 * This class forms the foundation of networking within the game.
 * A NetworkedData struct is sent around the clients via the server containing a
 * ROLE and CONTENT (up to 5x). The content is an integer array, stored in
 * "data_content" (1-5), and the role is an enum. The role describes the purpose
 * of the integers - E.G. number of action points assigned, etc. Not every
 * struct role requires all 5 integers, so they are initialised as zero (think
 * of this as null). The role must be checked when receiving a packet of
 * NetworkedData to work out its purpose and how many integers to check the
 * value of. In the enum "data_roles" each role is described with its integer
 * usage - refer to this when sending/receiving! This is a simple solution to
 * the complex datatype networking issue.
 *
 */

enum data_roles
{
  NO_ROLE,
  /* ^ This is the default role and should never be sent across the network. */

  PLAYER_MOVED_WITHIN_SHIP,
  /* ^ [0] = player's new room index
   *   [1] = player's previous room index
   *   [2] = player's action points after move
   *   [3] = player's ID
   *   [4] = N/A
   */

  PLAYER_ASSIGNED_ACTION_POINTS,
  /* ^ [0] = the card points were assigned to
   *   [1] = the number of action points assigned
   *   [2] = if the card was completed (0/1)
   *   [3] = N/A
   *   [4] = N/A
   */
};

struct NetworkedData
{
  data_roles data_role = data_roles::NO_ROLE;
  int data_content[5] = { 0, 0, 0, 0, 0 };
};

#endif // PROJECT_NETWORKEDDATA_H
