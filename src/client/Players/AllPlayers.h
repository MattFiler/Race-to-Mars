#include "CommunicationsPlayer.h"
#include "EngineerPlayer.h"
#include "MedicPlayer.h"
#include "PilotPlayer.h"

struct Players
{
  void render()
  {
    player_communications.render();
    player_engineer.render();
    player_medic.render();
    player_pilot.render();
  }
  Communications player_communications;
  Engineer player_engineer;
  Medic player_medic;
  Pilot player_pilot;
};