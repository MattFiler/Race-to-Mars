#include "CommunicationsPlayer.h"

Communications::Communications()
{
  retargetConfig("COMMUNICATIONS");
  counter.setSprite(getCounterSpritePath());
}