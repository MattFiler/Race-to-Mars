#ifndef PROJECT_CARD_H
#define PROJECT_CARD_H

#include "../Locator/ServiceLocator.h"
#include "client/UI/OnBoardObject.h"
#include "gamelib/Debug/DebugText.h"
#include "gamelib/NetworkedData/Cards.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <gamelib/FileHandler/FileHandler.h>
#include <gamelib/NetworkedData/Players.h>

/*
 *
 * RACE TO SPACE
 * The main cards instance to be used as a parent to all other card types (E.G.
 * issues, objectives)
 *
 */

class Card : public OnBoardObject
{
 public:
  Card();
  ~Card() = default;

 protected:
  /* Card Metadata (used in children) */
  std::string card_name = "Temp Name";
  std::string card_decription = "Temp desc";
  std::string ability_text = "Temp Ability Text";
  int action_points = 0;

  /* Engine Components */
  FileHandler file_handler;
  DebugText debug_text;

  /* Our Config */
  json card_config;
};

#endif // PROJECT_CARD_H
