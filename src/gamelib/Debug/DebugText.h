#ifndef PO_DEBUG
#define PO_DEBUG

#include <Engine/DebugPrinter.h>

/*
 *
 * RACE TO SPACE
 * Debug functionality to output a log to the console. Enabled/disabled at
 * runtime from a JSON key.
 *
 */

class DebugText
{
 public:
  DebugText() = default;
  ~DebugText() = default;

  // Print text with severity (-1=green, 0=white, 1=yellow, 2=red, 3=purple)
  void print(const std::string& debug_string, int severity = 0);

  static bool enabled;
};

#endif
