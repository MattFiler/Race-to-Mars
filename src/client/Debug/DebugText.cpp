#include "DebugText.h"
//#include "../Constants.h"

/* Print a string if debug outputs are enabled */
void DebugText::print(const std::string& debug_string)
{
  if (enabled)
  {
    ASGE::DebugPrinter{} << "PO_DEBUG: " << debug_string << std::endl;
  }
}

/* This value is modified to config definition at runtime */
bool DebugText::enabled = false;