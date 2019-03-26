#include <utility>

#include <utility>

#include "Card.h"

Card::Card()
{
  renderer = Locator::getRenderer();
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");
}

void Card::setCardName(std::string _item_name)
{
  this->m_card_name = std::move(_item_name);
}

void Card::setActionPoints(int _action_points)
{
  this->m_action_points = _action_points;
}

void Card::setSpritePath(std::string _sprite_path)
{
  this->m_sprite_path = std::move(_sprite_path);
}

void Card::setCardDescription(std::string _description_text)
{
  this->m_card_decription = std::move(_description_text);
}

void Card::setAbilityText(std::string _ability_text)
{
  this->m_ability_text = std::move(_ability_text);
}
