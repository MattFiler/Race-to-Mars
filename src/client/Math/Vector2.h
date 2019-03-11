#ifndef PROJECT_VECTOR2_H
#define PROJECT_VECTOR2_H

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

  Vector2& operator=(const Vector2&) { return *this; };

  bool operator==(Vector2& vector) { return (vector.x == x && vector.y == y); };
  Vector2 operator-(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.x = x - vector.x;
    return_vector.y = y - vector.y;
    return return_vector;
  };
  Vector2 operator+(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.x = x + vector.x;
    return_vector.y = y + vector.y;
    return return_vector;
  };
  Vector2 operator/(const float divider)
  {
    Vector2 return_vector;
    return_vector.x = x / divider;
    return_vector.y = y / divider;
    return return_vector;
  };
  Vector2 operator*(const float multiplier)
  {
    Vector2 return_vector;
    return_vector.x = x * multiplier;
    return_vector.y = y * multiplier;
    return return_vector;
  };

  float& x = m_x;
  float& y = m_y;

 private:
  float m_x;
  float m_y;
};

#endif // PROJECT_VECTOR2_H
