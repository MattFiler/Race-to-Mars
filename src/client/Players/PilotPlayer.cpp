#include "PilotPlayer.h"

Pilot::Pilot()
{
  retargetConfig("PILOT");
  counter.setSprite(getCounterSpritePath());
  counter.setDimensions(
    Vector2(config["counter_width"], config["counter_height"]));
}