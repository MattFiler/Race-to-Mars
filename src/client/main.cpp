#include "game.h"
#include <enetpp/client.h>
#include <gamelib/gamelib.h>
int main()
{
  RaceToSpace asge_game;
  asge_game.init();
  asge_game.run();

  return 0;
}