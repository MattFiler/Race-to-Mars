#include "CommunicationsPlayer.h"

Communications::Communications()
{
  retargetConfig("COMMUNICATIONS");
  counter.setSprite(getCounterSpritePath());
  counter.setDimensions(
    Vector2(config["counter_width"], config["counter_height"]));
}