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
  void setItemName(std::string _item_name);
  void setActionPoints(int _action_points);
  void setSpritePath(std::string _sprite_path);

 private:
  std::string item_name = "";
  int action_points = 0;
  std::string sprite_path = "";
};

#endif // PROJECT_ITEMCARD_H
