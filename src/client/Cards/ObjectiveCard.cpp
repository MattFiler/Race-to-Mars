#include "ObjectiveCard.h"

ObjectiveCard::ObjectiveCard(objective_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  auto card_type = static_cast<size_t>(_card_type);

  card_name = card_config["OBJECTIVECARDS"][card_type]["name"];
  // m_card_decription =
  // card_config["OBJECTIVECARDS"][card_type]["description"];
  // m_action_points =
  // card_config["OBJECTIVECARDS"][card_type]["action_points"];
  cardID = card_config["OBJECTIVECARDS"][card_type]["card_id"];
  setSprite(card_config["OBJECTIVECARDS"][card_type]["sprite_path"]);
}

void ObjectiveCard::setCardID(objective_cards _obj_card_id)
{
  cardID = _obj_card_id;
}
