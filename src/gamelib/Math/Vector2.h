#ifndef PROJECT_VECTOR2_H
#define PROJECT_VECTOR2_H

#include "../Constants.h"
#include <algorithm>
#include <math.h>

class Vector2
{
 public:
  Vector2() = default;
  Vector2(float _x, float _y, bool should_scale = true)
  {
    if (should_scale)
    {
      _x *= GameResolution::scale;
      _y *= GameResolution::scale;
    }
    m_x = _x;
    m_y = _y;
    is_corrected = should_scale;
  }

  void update(float _x, float _y, bool should_scale = true)
  {
    if (should_scale)
    {
      _x *= GameResolution::scale;
      _y *= GameResolution::scale;
    }
    m_x = _x;
    m_y = _y;
    is_corrected = should_scale;
  }

  bool operator==(Vector2& vector) { return (vector.x == x && vector.y == y); };
  Vector2 operator-(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.update(x - vector.x * GameResolution::scale,
                         y - vector.y * GameResolution::scale);
    return return_vector;
  };
  Vector2 operator+(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.update(x + vector.x * GameResolution::scale,
                         y + vector.y * GameResolution::scale);
    return return_vector;
  };
  Vector2 operator/(const float divider)
  {
    Vector2 return_vector;
    return_vector.update(x / divider, y / divider);
    return return_vector;
  };
  Vector2 operator/(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.update(x / vector.x, y / vector.y);
    return return_vector;
  };
  Vector2 operator*(const float multiplier)
  {
    Vector2 return_vector;
    return_vector.update(x * multiplier, y * multiplier);
    return return_vector;
  };
  Vector2 operator*(const Vector2& vector)
  {
    Vector2 return_vector;
    return_vector.update(x * vector.x, y * vector.y);
    return return_vector;
  };

  bool isCorrected() { return is_corrected; }

  const float& x = m_x;
  const float& y = m_y;

 private:
  float m_x;
  float m_y;
  bool is_corrected = true;
};

#endif // PROJECT_VECTOR2_H
