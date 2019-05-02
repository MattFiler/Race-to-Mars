#include <utility>

#include "ItemCard.h"

/* Create an Item Card */
ItemCard::ItemCard(item_cards card_index)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");
  auto card_type = static_cast<size_t>(card_index);

  card_name = card_config["ITEMCARDS"][card_type]["name"];
  action_points = card_config["ITEMCARDS"][card_type]["action_points"];
  std::string sprite_path = card_config["ITEMCARDS"][card_type]["sprite_path"];
  setSprite(sprite_path);
  cardID = card_config["ITEMCARDS"][card_type]["card_id"];
  player_class_type = card_config["ITEMCARDS"][card_type]["player_class"];
}

/* Set the card ID */
void ItemCard::setCardID(item_cards _item_card_id)
{
  cardID = _item_card_id;
}

/* Set active/inactive */
void ItemCard::setActive(bool _active)
{
  activated = _active;
}

/* Configure the variable APs from modifier cards */
void ItemCard::setActionPointVariable(int _ap)
{
  action_point_varibale = _ap;
}

/* Get this item's AP value for assigning to an issue */
int ItemCard::assignActionPoints()
{
  return action_points + action_point_varibale;
}