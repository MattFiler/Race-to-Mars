#ifndef PROJECT_PLAYER_H
#define PROJECT_PLAYER_H

#include "client/Board/PlayerCounter.h"
#include "client/Core/ServiceLocator.h"
#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/Localisation/Localisation.h"
#include <Engine/Renderer.h>
#include <json.hpp>
using json = nlohmann::json;

/*
 *
 * RACE TO SPACE
 * An instance of our generic player, to be used as a parent to all classes.
 *
 */

class Player
{
 public:
  Player();
  ~Player() = default;

  std::string getCounterSpritePath() { return config["counter_sprite"]; }
  int getStartingRoom() { return config["start_room"]; }
  std::string getFriendlyName()
  {
    return localiser.getString(config["friendly_name"]);
  }

  void render();

 protected:
  void retargetConfig(const std::string& name)
  {
    config = file_handler.loadConfigFromExisting(config, name);
  };

  ASGE::Renderer* renderer = nullptr;
  FileHandler file_handler;
  Localisation localiser;

  PlayerCounter counter;

 private:
  json config;
};

#endif // PROJECT_PLAYER_H
