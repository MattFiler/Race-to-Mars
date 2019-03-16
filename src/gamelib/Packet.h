//
// Created by jackk on 15/03/2019.
//

#ifndef PROJECT_PACKET_H
#define PROJECT_PACKET_H

#include <cstddef>
#include <vector>

class Packet
{
 public:
  explicit Packet(int _test);
  explicit Packet(const char* data);

  ~Packet() = default;

  std::size_t read_pos = 0;
  std::vector<char> packet_data;
  int test_int = 0;

  char* data(unsigned int& size) const;

  template<typename T>
  Packet& operator<<(const T& data)
  {
    auto size = sizeof(data);
    auto as_char = reinterpret_cast<const char*>(&data);
    packet_data.insert(packet_data.end(), as_char, as_char + size);
    return *this;
  }

  template<typename T>
  Packet& operator>>(T& data)
  {
    auto size = sizeof(T);
    auto as_type = reinterpret_cast<T*>(&packet_data[read_pos]);
    data = *as_type;
    read_pos += size;
    return *this;
  }
};
#endif // PROJECT_PACKET_H
