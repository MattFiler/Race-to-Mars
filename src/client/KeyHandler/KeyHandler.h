#ifndef PROJECT_KEYHANDLER_H
#define PROJECT_KEYHANDLER_H

#include "../Debug/DebugText.h"
#include <Engine/InputEvents.h>
#include <json.hpp>
using json = nlohmann::json;

/* User-defined keybinds (and handling for local use) */
struct KeyHandler
{
 public:
  KeyHandler() = default;

  // Initial setup of keybind values
  void setup(json& keybind_config) { config = keybind_config; };

  // Initialisation of key event handler
  void registerEvent(const ASGE::KeyEvent* new_event) { key_event = new_event; }

  // Check of specific key being released
  bool keyReleased(const std::string& keybind)
  {
    if (checkKeybind(keybind))
    {
      return (key_event->key == config[keybind] &&
              key_event->action == ASGE::KEYS::KEY_RELEASED);
    }
    return false;
  }

  // Check of specific key being pressed
  bool keyPressed(const std::string& keybind)
  {
    if (checkKeybind(keybind))
    {
      return (key_event->key == config[keybind] &&
              key_event->action == ASGE::KEYS::KEY_PRESSED);
    }
    return false;
  }

 private:
  bool checkKeybind(const std::string& keybind)
  {
    if (config[keybind].is_null())
    {
      debug_text.print("WARNING: ATTEMPTED TO ACCESS UNINITIALISED KEYBIND - " +
                       keybind);
      return false;
    }
    return true;
  }

  const ASGE::KeyEvent* key_event;
  static json config;
  DebugText debug_text;
};

#endif // PROJECT_KEYHANDLER_H
