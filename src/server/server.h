#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include "gamelib/Debug/DebugText.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include "gamelib/NetworkedData/Players.h"
#include <enetpp/server.h>
#include <gamelib/NetworkedData/Cards.h>
#include <random>
#include <vector>

/* A client on the server */
struct server_client
{
  unsigned int _uid;
  unsigned int get_id() const { return _uid; }
  int lobby_id = -1;
  int lobby_index = -1;
  int client_index = -1;
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
  std::vector<int> issue_deck;
  std::vector<int> item_deck;
  std::vector<int> objective_deck;
  int current_progress_index = 0;
  int player_that_started_id = -1;
  int currently_active_player = 0;

  // BELOW IS ALL TEMP SHIT READY FOR JACK TO ADD HIS CARD IMPLEMENTATIONS
  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };
};

class RaceToSpaceServer
{
 public:
  RaceToSpaceServer();
  ~RaceToSpaceServer();

  void initialise();
  void run();

  void connectToLobby(server_client& client);
  void disconnectFromLobby(int client_id);

 private:
  void sendData(server_client& client,
                unsigned int user_id,
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

  std::random_device r;
  std::seed_seq seed_seq{ r(), r(), r(), r(), r(), r() };
  std::mt19937 gen{ seed_seq };

  std::vector<Lobby> lobbies;
  int latest_lobby_id = 0;
};

#endif // PROJECT_SERVER_H
