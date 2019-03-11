#ifndef PROJECT_VECTOR3_H
#define PROJECT_VECTOR3_H

class Vector3
{
 public:
  Vector3(float _x, float _y, float _z)
  {
    m_x = _x;
    m_y = _y;
    m_z = _z;
  }

  const float& x = m_x;
  const float& y = m_y;
  const float& z = m_z;

 private:
  float m_x;
  float m_y;
  float m_z;
};

#endif // PROJECT_VECTOR3_H
