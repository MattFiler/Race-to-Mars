//
// Created by j42-king on 30/03/2019.
//

#include "SimpleMath.h"
#include <chrono>
#include <random>

float SimpleMath::generateRandFloat(float min, float max)
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator(seed);
  std::uniform_real_distribution<float> distr{ min, max };
  return distr(generator);
}

int SimpleMath::generateRandInt(int min, int max)
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 generator(seed);
  std::uniform_int_distribution<int> distr{ min, max };
  return distr(generator);
}

float SimpleMath::lerp(float p1, float p2, float percentage)
{
  return p1 + percentage * (p2 - p1);
}

float SimpleMath::distance(float p1, float p2)
{
  return std::sqrt(p1 * p1 + p2 * p2);
}