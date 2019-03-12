#ifndef PROJECT_NETWORKCONNECTION_H
#define PROJECT_NETWORKCONNECTION_H

#include <enetpp/client.h>

class RaceToSpace;

class NetworkConnection
{
 public:
  NetworkConnection() = default;
  ~NetworkConnection();

  void connectToServer(const std::string& hostname, enet_uint16 port);

  void startListening(RaceToSpace* game_instance);
  void networkLoop();
  void networkMessageDebug();

  enetpp::client* getClient() { return &client; };
  const bool& connected = has_connected;

 private:
  void connection();
  void disconnection();
  void data(const enet_uint8* data, size_t data_size);

  bool has_connected = false;
  RaceToSpace* game = nullptr;

  enetpp::client client;
  std::atomic<bool> exiting = false;
  std::queue<std::string> msg_queue;
  std::mutex msg_queue_mtx;
};

#endif // PROJECT_NETWORKCONNECTION_H
