#ifndef PROJECT_SERVERCLIENT_H
#define PROJECT_SERVERCLIENT_H

/* A client on the server */
struct server_client
{
  unsigned int _uid;
  unsigned int get_id() const { return _uid; }
  int lobby_id = -1;
  int lobby_index = -1;
  int client_index = -1;
};

#endif // PROJECT_SERVERCLIENT_H
