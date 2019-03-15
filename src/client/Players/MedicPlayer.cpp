#include "MedicPlayer.h"

Medic::Medic()
{
  retargetConfig("MEDIC");
  counter.setSprite(getCounterSpritePath());
  counter.setDimensions(
    Vector2(config["counter_width"], config["counter_height"]));
}