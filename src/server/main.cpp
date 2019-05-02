#include "server.h"
#include <enetpp/client.h>
#include <gamelib/gamelib.h>
#include <memory>
int main()
{
  std::unique_ptr<RaceToSpaceServer> server =
    std::make_unique<RaceToSpaceServer>();

  server->initialise();
  server->run();
  return 0;
}