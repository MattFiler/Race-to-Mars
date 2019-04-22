//
// Created by jackk on 22/04/2019.
//

#ifndef PROJECT_CHATMSG_H
#define PROJECT_CHATMSG_H

#include <chrono>
#include <iostream>

class ChatMsg
{
 public:
  ChatMsg() = default;
  ChatMsg(std::string user, std::string msg);

  ChatMsg(const char* data);

  ~ChatMsg() = default;
  char* data(unsigned int& size) const;
  const std::string& getMsg() const;
  const std::string& getUsername() const;

 private:
  std::string msg_text;
  std::string username;
};

#endif // PROJECT_CHATMSG_H
