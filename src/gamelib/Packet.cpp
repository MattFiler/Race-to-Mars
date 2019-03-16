//
// Created by jackk on 15/03/2019.
//

#include "Packet.h"

Packet::Packet(int _test) {}

Packet::Packet(const char* data) {}

char* Packet::data(unsigned int& size) const
{
  size = char* data = new char[size];

  // return pkt ready to send to server.
  return data;
}
