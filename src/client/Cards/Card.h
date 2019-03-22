#ifndef PROJECT_CARD_H
#define PROJECT_CARD_H

#include "../Core/ServiceLocator.h"
#include <Engine/Renderer.h>
#include <Engine/Sprite.h>

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

  void setCardName(std::string _item_name);
  void setActionPoints(int _action_points);
  void setSpritePath(std::string _sprite_path);
  void setCardDescription(std::string _description_text);
  void setAbilityText(std::string _ability_test);

 protected:
  ASGE::Renderer* renderer = nullptr;

  std::string m_card_name = "";
  std::string m_card_decription = "";
  std::string m_sprite_path = "";
  std::string m_ability_text = "";

  int m_action_points = 0;

 private:
  /* Stuff here */
};

#endif // PROJECT_CARD_H
