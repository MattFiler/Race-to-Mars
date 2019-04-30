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
