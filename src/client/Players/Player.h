#ifndef PROJECT_PLAYER_H
#define PROJECT_PLAYER_H

#include "client/Board/PlayerCounter.h"
#include "client/Locator/ServiceLocator.h"
#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/Localisation/Localisation.h"
#include <Engine/Renderer.h>
#include <gamelib/Math/SimpleMath.h>
#include <gamelib/NetworkedData/Players.h>
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
  // ScaledSprite* getCounterSprite() { return counter.getSprite(); } -
  // shouldn't be needed as we provide a render function and necessary handles
  // for moving, etc
  ScaledSprite* getLobbySprite() { return lobby_sprite; }
  ScaledSprite* getGameTabSprite() { return game_tab_sprite; }

  void setPos(Vector2 new_pos) { counter.setPosition(new_pos); }
  Vector2 getPos() { return counter.getPosition(); }

  float getWidth() { return counter.getSprite()->height(); }
  float getHeight() { return counter.getSprite()->width(); }
  void setDiceRolls(int _dice_roll) { amount_of_rolls += _dice_roll; }
  void setMaxItems(int _max_items) { max_items = _max_items; }
  void setChasingChicken(bool _chasing_chicken)
  {
    chasing_chicken = _chasing_chicken;
  }
  void setHeldItems(int _item_increase) { held_items += _item_increase; }
  void setUsedAPThisTurn(bool _used_ap) { ap_not_assigned = _used_ap; }
  void setReplenishItems(bool _replenish_items)
  {
    replenish_items = _replenish_items;
  }

  int getMaxItems() { return max_items; }
  int getHeldItemAmount() { return held_items; }
  bool getUsedApThisTurn() { return ap_not_assigned; }
  bool getReplenishItems() { return replenish_items; }

  void makeUninitialised()
  {
    retargetConfig("DEFAULT");
    setup();
  }

  void render(game_global_scenes game_scene);
  int getDiceRoll();

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

  int current_action_points = 0;
  int held_items = 0;
  int max_items = 2;
  int amount_of_rolls = 1;
  bool chasing_chicken = false;
  bool ap_not_assigned = true;
  bool replenish_items = false;

 private:
  SimpleMath math;
};

#endif // PROJECT_PLAYER_H
