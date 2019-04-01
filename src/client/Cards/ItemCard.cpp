#include <utility>

#include "ItemCard.h"

void ItemCard::setCardID(item_cards _item_card_id)
{
  m_cardID = _item_card_id;
}

void ItemCard::setActive(bool _active)
{
  m_activated = _active;
}

void ItemCard::setActionPointVariable(int _ap)
{
  m_action_point_varibale = _ap;
}

int ItemCard::assignActionPoints()
{
  return m_action_points + m_action_point_varibale;
}
