#include <utility>

#include "Card.h"

Card::Card()
{
  renderer = Locator::getRenderer();
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");
}

void Card::setCardName(std::string _item_name)
{
  this->m_card_name = _item_name;
}

void Card::setActionPoints(int _action_points)
{
  this->m_action_points = _action_points;
}

void Card::setSpritePath(std::string _sprite_path)
{
  m_sprite_path = _sprite_path;
}

void Card::setCardDescription(std::string _description_text)
{
  m_card_decription = _description_text;
}

void Card::setAbilityText(std::string _ability_text)
{
  m_ability_text = _ability_text;
}

void Card::render()
{
  renderer->renderSprite(*m_card_sprite);
}

void Card::setCardSize() {}
