#include "MedicPlayer.h"

Medic::Medic()
{
  retargetConfig("MEDIC");
  counter.setSprite(getCounterSpritePath());
}