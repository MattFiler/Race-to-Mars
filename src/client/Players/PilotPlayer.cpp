#include "PilotPlayer.h"

Pilot::Pilot()
{
  retargetConfig("PILOT");
  counter.setSprite(getCounterSpritePath());
}