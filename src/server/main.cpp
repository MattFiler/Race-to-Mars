#include <enetpp/client.h>
#include <gamelib/gamelib.h>
int main()
{
  enetpp::global_state::get().initialize();
  enetpp::global_state::get().deinitialize();
  return 0;
}