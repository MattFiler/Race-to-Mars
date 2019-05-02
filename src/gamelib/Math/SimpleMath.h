#ifndef PROJECT_SIMPLEMATH_H
#define PROJECT_SIMPLEMATH_H

struct SimpleMath
{
  // Construction
  SimpleMath() = default;
  ~SimpleMath() = default;

  // Operations
  float generateRandFloat(float min, float max);
  int generateRandInt(int min, int max);
  float lerp(float p1, float p2, float percentage);
  float distance(float p1, float p2);

  // Data
  float count = 0.0f;
  float timeout = 0.0f;
};

#endif // PROJECT_SIMPLEMATH_H
