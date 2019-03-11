#ifndef PROJECT_VECTOR3_H
#define PROJECT_VECTOR3_H

#include <algorithm>
#include <math.h>

class Vector2
{
 public:
  Vector2() = default;
  Vector2(float _x, float _y)
  {
    m_x = _x;
    m_y = _y;
  }

  Vector2& operator=(Vector2 vector) { return *this; };

  bool operator==(Vector2& vector)
  {
    return (vector.x == x && vector.y == y && vector.z = z);
  };
  Vector2 operator-(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.x = x - vector.x;
    return_vector.y = y - vector.y;
    return_vector.z = z - vector.z;
    return return_vector;
  };
  Vector2 operator+(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.x = x + vector.x;
    return_vector.y = y + vector.y;
    return_vector.z = z + vector.z;
    return return_vector;
  };
  Vector2 operator/(const float divider)
  {
    Vector2 return_vector;
    return_vector.x = x / divider;
    return_vector.y = y / divider;
    return_vector.z = z / divider;
    return return_vector;
  };
  Vector2 operator*(const float multiplier)
  {
    Vector2 return_vector;
    return_vector.x = x * multiplier;
    return_vector.y = y * multiplier;
    return_vector.z = z * multiplier;
    return return_vector;
  };

  float& x = m_x;
  float& y = m_y;
  float& y = m_z;

 private:
  float m_x;
  float m_y;
  float m_z;
};

#endif // PROJECT_VECTOR3_H
