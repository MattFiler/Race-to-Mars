//
// Created by jackk on 22/04/2019.
//

#include "ChatMsg.h"
#include <cstring>

ChatMsg::ChatMsg(const std::string msg) : msg_text(msg) {}

ChatMsg::ChatMsg(const char* data)
{
  msg_text = data;
  // auto stamp_loc = data + + (msg_text.length() + 1);
  // memcpy(msg_text, stamp_loc, sizeof(std::time_t));
}

const std::string& ChatMsg::getMsg() const
{
  return msg_text;
}

char* ChatMsg::data(unsigned int& size) const
{
  auto txt_length = msg_text.length() + 1;
  char* data = new char[size];
  memcpy(static_cast<void*>(&data[0]), msg_text.data(), txt_length);
  return data;
}
