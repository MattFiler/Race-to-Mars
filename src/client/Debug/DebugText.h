#ifndef PO_DEBUG
#define PO_DEBUG

#include <Engine/DebugPrinter.h>

class DebugText
{
 public:
  DebugText() = default;
  ~DebugText() = default;

  void print(const std::string& debug_string);

  static bool enabled;
};

#endif
