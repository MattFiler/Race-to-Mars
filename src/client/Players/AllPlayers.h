#include "../NetworkedData/PlayerData.h"
#include "CommunicationsPlayer.h"
#include "EngineerPlayer.h"
#include "MedicPlayer.h"
#include "PilotPlayer.h"
#include "gamelib/Constants.h"
#include <gamelib/NetworkedData/Players.h>

struct Players
{
  // cppcheck-suppress *
  Players() { player_uninitialised.makeUninitialised(); }

  // Render (scene dependant)
  void render(game_global_scenes game_scene)
  {
    player_communications.render(game_scene);
    player_engineer.render(game_scene);
    player_pilot.render(game_scene);
    player_medic.render(game_scene);
  }

  // Get player class pointer by enum
  Player* getPlayer(player_classes _class)
  {
    switch (_class)
    {
      case player_classes::ENGINEER:
      {
        return &player_engineer;
      }
      case player_classes::PILOT:
      {
        return &player_pilot;
      }
      case player_classes::MEDIC:
      {
        return &player_medic;
      }
      case player_classes::COMMUNICATIONS:
      {
        return &player_communications;
      }
      default:
      {
        return &player_uninitialised;
      }
    }
  }

  // Class instances
  Communications player_communications;
  Engineer player_engineer;
  Medic player_medic;
  Pilot player_pilot;
  Player player_uninitialised;

  // Data needed in game board
  int my_player_index = -1;
  int current_progress_index = 0;

  // Class client data
  LobbyPlayer players[4];

  // Did join in progress? If so, initiate sync.
  bool joined_in_progress = false;
};