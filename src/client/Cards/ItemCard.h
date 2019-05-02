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
  explicit ItemCard(item_cards item_index);

  void setCardID(item_cards _item_card_id);
  item_cards getCardID() { return cardID; };

  void setActive(bool _active);

  void setActionPointVariable(int _action_point);
  int assignActionPoints();

  player_classes getItemPlayerType() { return player_class_type; }

 private:
  /* Item Metadata */
  item_cards cardID = item_cards::ITEM_PLACEHOLDER;
  player_classes player_class_type = player_classes::UNASSIGNED;
  bool activated = true;

  /* AP Modifiers */
  int action_point_varibale = 0;
};

#endif // PROJECT_ITEMCARD_H
