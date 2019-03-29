#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include "gamelib/Debug/DebugText.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include "gamelib/NetworkedData/Players.h"
#include "server/Structs/Lobby.h"
#include "server/Structs/ServerClient.h"
#include <enetpp/server.h>
#include <gamelib/NetworkedData/Cards.h>
#include <random>
#include <vector>

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

  Lobby* getLobbyByID(int lobby_id);

  enetpp::server<server_client> network_server;
  unsigned int next_uid = 0;
  DebugText debug_text;
  int port = 8888;
  int max_lobby_size = 4;

  std::mt19937 gen;

  std::vector<Lobby> lobbies;
  int latest_lobby_id = 0;
};

#endif // PROJECT_SERVER_H
