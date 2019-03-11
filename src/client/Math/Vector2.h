#ifndef PROJECT_VECTOR2_H
#define PROJECT_VECTOR2_H

class Vector2
{
 public:
  Vector2(float _x, float _y)
  {
    m_x = _x;
    m_y = _y;
  }

  const float& x = m_x;
  const float& y = m_y;

 private:
  float m_x;
  float m_y;
};

#endif // PROJECT_VECTOR2_H
