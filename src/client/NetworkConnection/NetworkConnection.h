#ifndef PROJECT_NETWORKCONNECTION_H
#define PROJECT_NETWORKCONNECTION_H

#include "gamelib/NetworkedData/NetworkedData.h"
#include <enetpp/client.h>
#include <gamelib/Packet.h>

class RaceToSpace;

class NetworkConnection
{
 public:
  NetworkConnection() = default;
  ~NetworkConnection();

  void connectToServer(const std::string& hostname, enet_uint16 port);
  void startListening(RaceToSpace* game_instance);

  void sendData(data_roles _role,
                int _content_1,
                int _content_2 = 0,
                int _content_3 = 0,
                int _content_4 = 0,
                int _content_5 = 0);

  enetpp::client* getClient() { return &client; };

 private:
  void networkLoop();
  void networkMessageDebug();

  std::queue<Packet>* getPacketQueue() { return &pkt_queue; };
  std::mutex* getMutex() { return &pkt_queue_mtx; };

  RaceToSpace* game = nullptr;
  enetpp::client client;
  std::atomic<bool> exiting = false;
  std::queue<Packet> pkt_queue;
  std::mutex pkt_queue_mtx;
  std::queue<std::string> msg_queue_debug;
  std::mutex msg_queue_mtx;
  std::mutex msg_queue_debug_mtx;
};

#endif // PROJECT_NETWORKCONNECTION_H
