#ifndef PROJECT_NETWORKCONNECTION_H
#define PROJECT_NETWORKCONNECTION_H

#include <enetpp/client.h>
#include <gamelib/ChatMsg.h>
#include <gamelib/Packet.h>

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
  void input();

  enetpp::client* getClient() { return &client; };

  std::mutex* getMutex() { return &pkt_queue_mtx; };
  std::queue<Packet>* getPacketQueue() { return &pkt_queue; };

 private:
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
