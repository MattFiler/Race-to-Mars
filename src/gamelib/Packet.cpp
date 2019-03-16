//
// Created by jackk on 15/03/2019.
//

#include "Packet.h"

Packet::Packet(const char* data) {}

char* Packet::data(unsigned int& size) const
{
  // assign size ref to the current packet_data size
  size = static_cast<unsigned int>(packet_data.size());

  // Create vector of char to store data in using size of packet_data.
  char* data = new char[size];

  // return pkt data ready to send to to client
  return data;
}
