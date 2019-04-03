#include <utility>

#include "ItemCard.h"

void ItemCard::setCardID(item_cards _item_card_id)
{
  cardID = _item_card_id;
}

void ItemCard::setActive(bool _active)
{
  activated = _active;
}

void ItemCard::setActionPointVariable(int _ap)
{
  action_point_varibale = _ap;
}

int ItemCard::assignActionPoints()
{
  return action_points + action_point_varibale;
}
