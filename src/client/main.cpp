#include "game.h"
#include <enetpp/client.h>
#include <gamelib/gamelib.h>
int main()
{
  enetpp::global_state::get().initialize();

  RaceToSpace asge_game;
  asge_game.init();
  asge_game.run();

  enetpp::global_state::get().deinitialize();
  return 0;
}