#ifndef PROJECT_PACKET_H
#define PROJECT_PACKET_H

#include <cstddef>
#include <cstring>
#include <enet/types.h>
#include <string>
#include <vector>

class Packet
{
 public:
  explicit Packet() = default;
  // explicit Packet(const char *data, size_t size);
  Packet(const enet_uint8* data, size_t length);
  ~Packet() = default;

  std::size_t read_pos = 0;
  std::vector<char> packet_data;

  char* data();
  unsigned long long int length();

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

  // functions for strings.
  //  Packet& operator<<(const std::string& str)
  //  {
  //    auto data = str.data();
  //    auto length = str.length() + 1; // +1 for null
  //    packet_data.insert(packet_data.end(), data, data + length);
  //    return *this;
  //  }
  //
  //  Packet& operator>>(std::string& str)
  //  {
  //    size_t length;
  //    length = strlen(&packet_data[read_pos]);
  //    str = std::string(&packet_data[read_pos], length);
  //    read_pos += packet_data.size();
  //    return *this;
  //  }
};
#endif // PROJECT_PACKET_H
