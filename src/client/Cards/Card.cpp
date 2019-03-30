#include <utility>

#include "Card.h"

Card::Card()
{
  renderer = Locator::getRenderer();
}

void Card::render()
{
  renderer->renderSprite(*m_card_sprite->getSprite());
}

void Card::setCardName(const std::string& _item_name)
{
  m_card_name = _item_name;
}

void Card::setActionPoints(int _action_points)
{
  m_action_points = _action_points;
}

void Card::setSpritePath(const std::string& _sprite_path)
{
  m_sprite_path = _sprite_path;
}

void Card::setCardDescription(const std::string& _description_text)
{
  m_card_decription = _description_text;
}

void Card::setAbilityText(const std::string& _ability_text)
{
  m_ability_text = _ability_text;
}

void Card::setCardSize() {}
