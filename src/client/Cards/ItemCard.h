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
  item_cards getCardID() { return cardID; };
  void setActive(bool _active);
  void setActionPointVariable(int _action_point);

  int assignActionPoints();

 private:
  item_cards cardID = item_cards ::ITEM_PLACEHOLDER;
  bool activated = true;
  int action_points = 5;
  int action_point_varibale = 0;
};

#endif // PROJECT_ITEMCARD_H
