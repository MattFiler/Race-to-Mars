#ifndef PROJECT_NETWORKCONNECTION_H
#define PROJECT_NETWORKCONNECTION_H

#include "gamelib/NetworkedData/DataShare.h"
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

  void sendData(DataShare& data);

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
