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

  void print(const std::string& debug_string);

  static bool enabled;
};

#endif
