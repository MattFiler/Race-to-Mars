//
// Created by j42-king on 15/03/2019.
//

#include "ChatMsg.h"
#include <cstring>

ChatMsg::ChatMsg(std::string user, std::string msg, std::time_t stamp) :
  username(user), msg_text(msg), timestamp(stamp)
{
}

ChatMsg::ChatMsg(const char* data)
{
  username = data;
  msg_text = data + username.length() + 1;
  auto stamp_loc = data + (username.length() + 1) + (msg_text.length() + 1);
  memcpy(&timestamp, stamp_loc, sizeof(std::time_t));
}

const std::string& ChatMsg::getMsg() const
{
  return msg_text;
}

const std::time_t& ChatMsg::getStamp() const
{
  return timestamp;
}

const std::string& ChatMsg::getUsername() const
{
  return username;
}

char* ChatMsg::data(unsigned int& size) const
{
  const int username_length = static_cast<int>(username.length() + 1);
  const int txt_length = static_cast<int>(msg_text.length() + 1);
  const int timestamp_size = sizeof(std::time_t);
  size =
    static_cast<unsigned int>(username_length + txt_length + timestamp_size);
  char* data = new char[size];
  memcpy(static_cast<void*>(&data[0]),
         username.data(),
         static_cast<size_t>(username_length));
  memcpy(static_cast<void*>(&data[username_length]),
         msg_text.data(),
         static_cast<size_t>(txt_length));
  memcpy(static_cast<void*>(&data[username_length + txt_length]),
         &timestamp,
         timestamp_size);
  return data;
}
