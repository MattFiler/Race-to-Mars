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
  std::string getLobbySpritePath() { return config["lobby_sprite"]; }
  std::string getGameTabSpritePath() { return config["ingame_tab_sprite"]; }
  int getStartingRoom() { return config["start_room"]; }
  std::string getFriendlyName()
  {
    return localiser.getString(config["friendly_name"]);
  }
  ScaledSprite* getLobbySprite() { return lobby_sprite; }
  ScaledSprite* getGameTabSprite() { return game_tab_sprite; }

  void makeUninitialised()
  {
    retargetConfig("DEFAULT");
    setup();
  }

  void render(game_global_scenes game_scene);

 protected:
  void retargetConfig(const std::string& name)
  {
    config = file_handler.loadConfigFromExisting(config, name);
  };
  void setup();

  ASGE::Renderer* renderer = nullptr;
  FileHandler file_handler;
  Localisation localiser;
  PlayerCounter counter;
  ScaledSprite* lobby_sprite = nullptr;
  ScaledSprite* game_tab_sprite = nullptr;
  json config;
};

#endif // PROJECT_PLAYER_H
