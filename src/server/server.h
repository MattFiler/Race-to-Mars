#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include "gamelib/Debug/DebugText.h"
#include "gamelib/NetworkedData/DataShare.h"
#include "gamelib/NetworkedData/NetworkedData.h"
#include "gamelib/NetworkedData/Players.h"
#include "server/Structs/Lobby.h"
#include "server/Structs/ServerClient.h"
#include <enetpp/server.h>
#include <gamelib/Math/SimpleMath.h>
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
  void disconnectFromLobby(int client_id, bool alert_lobby = false);
  void initLobbyDecks();

 private:
  void sendToAll(server_client& client, DataShare& data);

  void handleReceivedData(DataShare& data_to_send, server_client& client);

  void clientJoinLobby(server_client& client);
  void endTurn(server_client& client);
  void syncClient(server_client& client);
  void clientReadyUp(DataShare& data_to_send, server_client& client);
  void clientPointsChange(DataShare& data_to_send, server_client& client);
  void clientMoved(DataShare& data_to_send, server_client& client);
  void clientProgressChange(DataShare& data_to_send, server_client& client);
  void clientRequestsItem(DataShare& data_to_send, server_client& client);
  void clientSolvedIssueCard(DataShare& data_to_send, server_client& client);
  void clientRequestsObjective(DataShare& data_to_send, server_client& client);
  void clientFreeMovement(DataShare& data_to_send, server_client& client);
  void chatMsg(DataShare& data_to_send, server_client& client);

  void sendData(server_client& client, unsigned int user_id, DataShare& data);

  Lobby* getLobbyByID(int lobby_id);

  enetpp::server<server_client> network_server;
  unsigned int next_uid = 0;
  DebugText debug_text;
  int port = 8888;
  int max_lobby_size = 4;

  std::vector<Lobby> lobbies;
  int latest_lobby_id = 0;
  SimpleMath simplemaths;
};

#endif // PROJECT_SERVER_H
