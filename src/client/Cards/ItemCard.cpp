#include <utility>

#include "ItemCard.h"

void ItemCard::setItemName(std::string _item_name)
{
  this->item_name = std::move(_item_name);
}

void ItemCard::setActionPoints(int _action_points)
{
  this->action_points = _action_points;
}

void ItemCard::setSpritePath(std::string _sprite_path)
{
  this->sprite_path = std::move(_sprite_path);
}
