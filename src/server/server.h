#ifndef PROJECT_SERVER_H
#define PROJECT_SERVER_H

#include <enetpp/server.h>

struct server_client
{
  unsigned int _uid;
  unsigned int get_id() const { return _uid; }
};

class RaceToSpaceServer
{
 public:
  RaceToSpaceServer();
  ~RaceToSpaceServer();

  void initialise();
  void run();

 private:
  enetpp::server<server_client> network_server;
  unsigned int next_uid = 0;
};

#endif // PROJECT_SERVER_H
