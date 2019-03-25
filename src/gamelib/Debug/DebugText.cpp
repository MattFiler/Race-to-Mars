#include "DebugText.h"
#include "gamelib/Constants.h"
#include <chrono>
#include <ctime>

/* Print a string if debug outputs are enabled */
void DebugText::print(const std::string& debug_string)
{
  if (enabled)
  {
    time_t now = time(0);
    std::string now_time = ctime(&now);
    ASGE::DebugPrinter{} << now_time.substr(0, now_time.length() - 1) << " - "
                         << debug_string << std::endl;
  }
}

/* This value is modified to config definition at runtime */
bool DebugText::enabled = false;