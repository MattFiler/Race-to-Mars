#include "Packet.h"

Packet::Packet(const enet_uint8* data, size_t length)
{
  packet_data.insert(packet_data.end(), data, data + length);
}

char* Packet::data()
{
  return packet_data.data();
}

unsigned long long int Packet::length()
{
  return packet_data.size();
}
