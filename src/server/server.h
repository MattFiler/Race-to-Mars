#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include "gamelib/Debug/DebugText.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include "gamelib/NetworkedData/Players.h"
#include <enetpp/server.h>
#include <vector>

/* A client on the server */
struct server_client
{
  unsigned int _uid;
  unsigned int get_id() const { return _uid; }
  int lobby_id = -1;
  int lobby_index = -1;
};

/* A game lobby */
struct Lobby
{
  Lobby() = delete;
  explicit Lobby(int _id) { lobby_id = _id; }
  int lobby_id = -1;
  int user_count = 0;
  int user_ids[4] = { -1, -1, -1, -1 };
  bool users_ready[4] = { false, false, false, false };
  player_classes user_classes[4] = { player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED,
                                     player_classes::UNASSIGNED };
};

class RaceToSpaceServer
{
 public:
  RaceToSpaceServer();
  ~RaceToSpaceServer();

  void initialise();
  void run();

 private:
  void sendData(unsigned int user_id,
                data_roles _role,
                int _content_1,
                int _content_2 = 0,
                int _content_3 = 0,
                int _content_4 = 0,
                int _content_5 = 0,
                int _content_6 = 0,
                int _content_7 = 0,
                int _content_8 = 0,
                int _content_9 = 0,
                int _content_10 = 0);

  enetpp::server<server_client> network_server;
  unsigned int next_uid = 0;
  DebugText debug_text;
  int port = 8888;
  int max_lobby_size = 4;

  std::vector<Lobby> lobbies;
  int latest_lobby_id = 0;
};

#endif // PROJECT_SERVER_H
