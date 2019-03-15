#include "EngineerPlayer.h"

Engineer::Engineer()
{
  retargetConfig("ENGINEER");
  counter.setSprite(getCounterSpritePath());
  counter.setDimensions(
    Vector2(config["counter_width"], config["counter_height"]));
}