#ifndef PROJECT_ITEMCARD_H
#define PROJECT_ITEMCARD_H

#include "Card.h"

/*
 *
 * RACE TO SPACE
 * Item cards, acquired by buying a random stab at the pack from the supply
 * room.
 *
 */

class ItemCard : public Card
{
 public:
  void setCardID(item_cards _item_card_id);

 private:
  item_cards m_cardID = item_cards ::ITEM_PLACEHOLDER;
};

#endif // PROJECT_ITEMCARD_H
