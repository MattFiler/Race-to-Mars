#ifndef PROJECT_LOCALISATION_H
#define PROJECT_LOCALISATION_H

#include "../FileHandler/FileHandler.h"
#include "gamelib/Debug/DebugText.h"
#include <json.hpp>
using json = nlohmann::json;

struct Localisation
{
  // Setup the localiser
  void configure(const std::string& language)
  {
    language_definition = language;
    language_config = file_handler.loadConfig("localisation.json", language);
  }
  // Get a localised string
  std::string getString(const std::string& id)
  {
    if (language_config[id].is_string())
    {
      return language_config[id];
    }
    debug_text.print("COULDN'T FIND A LOCALISATION FOR '#" + id + "'", 1);
    return "#" + id;
  }
  // Get the current language option
  std::string getLanguage() { return language_definition; }

 private:
  FileHandler file_handler;
  static json language_config;
  static std::string language_definition;
  DebugText debug_text;
};

#endif // PROJECT_LOCALISATION_H
