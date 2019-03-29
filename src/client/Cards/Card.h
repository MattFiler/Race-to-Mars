#ifndef PROJECT_CARD_H
#define PROJECT_CARD_H

#include "../Core/ServiceLocator.h"
#include "client/UI/OnBoardObject.h"
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

class Card
{
 public:
  Card();
  ~Card() = default;

  void render();

  void setCardName(const std::string& _item_name);
  void setSpritePath(const std::string& _sprite_path);
  void setCardDescription(const std::string& _description_text);
  void setAbilityText(const std::string& _ability_text);
  void setActionPoints(int _action_points);
  void setCardSize();

  std::string getSprtiepath() { return m_sprite_path; }

 protected:
  ASGE::Renderer* renderer = nullptr;

  std::string m_card_name = "Temp Name";
  std::string m_card_decription = "Temp desc";
  std::string m_sprite_path = "data/UI/CARD_IMAGES/medic_card.png\"";
  std::string m_ability_text = "Temp Ability Text";
  int m_action_points = 0;

  ASGE::Sprite* m_card_sprite = nullptr;

  FileHandler file_handler;
  json card_config;

 private:
  /* Stuff here */
};

#endif // PROJECT_CARD_H
