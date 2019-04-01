#ifndef PROJECT_CARD_H
#define PROJECT_CARD_H

#include "../Core/ServiceLocator.h"
#include "client/UI/OnBoardObject.h"
#include "gamelib/Debug/DebugText.h"
#include "gamelib/NetworkedData/Cards.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>
#include <gamelib/FileHandler/FileHandler.h>

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
  Card() = default;
  ~Card() = default;

 protected:
  std::string card_name = "Temp Name";
  std::string card_decription = "Temp desc";
  std::string ability_text = "Temp Ability Text";
  int action_points = 0;

  FileHandler file_handler;
  json card_config;
  DebugText debug_text;
};

#endif // PROJECT_CARD_H
