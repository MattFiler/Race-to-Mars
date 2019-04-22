//
// Created by jackk on 22/04/2019.
//

#include "ChatMsg.h"
#include <cstring>

ChatMsg::ChatMsg(const std::string user, const std::string msg) :
  msg_text(msg), username(user)
{
}

ChatMsg::ChatMsg(const char* data)
{
  username = data;
  msg_text = data + (username.length() + 1);
}

const std::string& ChatMsg::getMsg() const
{
  return msg_text;
}

const std::string& ChatMsg::getUsername() const
{
  return username;
}

char* ChatMsg::data(unsigned int& size) const
{
  auto username_length = username.length() + 1;
  auto txt_length = msg_text.length() + 1;
  char* data = new char[size];
  memcpy(static_cast<void*>(&data[0]), username.data(), username_length);
  memcpy(
    static_cast<void*>(&data[username_length]), msg_text.data(), txt_length);
  return data;
}
