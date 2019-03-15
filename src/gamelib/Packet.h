//
// Created by jackk on 15/03/2019.
//

#ifndef PROJECT_PACKET_H
#define PROJECT_PACKET_H

#include <cstddef>
#include <vector>

template<typename T>
class Packet
{
 public:
  Packet() = default;
  ~Packet() = default;

  std::size_t read_pos = 0;
  std::vector<char> packet_data;
};

#endif // PROJECT_PACKET_H
