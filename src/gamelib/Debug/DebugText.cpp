#include "DebugText.h"
#include <chrono>
#include <ctime>
#include <termcolor.hpp>

/* Print a string if debug outputs are enabled */
void DebugText::print(const std::string& debug_string, int severity)
{
  if (enabled)
  {
    // Work out time to log
    time_t now = time(0);
    std::string now_time = ctime(&now);

    // Output by colour
    switch (severity)
    {
      case -1:
      {
        ASGE::DebugPrinter{} << now_time.substr(0, now_time.length() - 1)
                             << " - " << termcolor::green << debug_string
                             << std::endl;
        break;
      }
      case 0:
      {
        ASGE::DebugPrinter{} << now_time.substr(0, now_time.length() - 1)
                             << " - " << termcolor::white << debug_string
                             << std::endl;
        break;
      }
      case 1:
      {
        ASGE::DebugPrinter{} << now_time.substr(0, now_time.length() - 1)
                             << " - " << termcolor::yellow << debug_string
                             << std::endl;
        break;
      }
      case 2:
      {
        ASGE::DebugPrinter{} << now_time.substr(0, now_time.length() - 1)
                             << " - " << termcolor::red << debug_string
                             << std::endl;
        break;
      }
    }
  }
}

/* This value is modified to config definition at runtime */
bool DebugText::enabled = false;