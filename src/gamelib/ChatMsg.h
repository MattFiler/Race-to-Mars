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
  ChatMsg(std::string msg);

  explicit ChatMsg(const char* data);

  ~ChatMsg() = default;
  char* data(unsigned int& size) const;
  const std::string& getMsg() const;
  void setMsg(const std::string& new_msg) { msg_text = new_msg; }

 private:
  std::string msg_text;
};

#endif // PROJECT_CHATMSG_H
