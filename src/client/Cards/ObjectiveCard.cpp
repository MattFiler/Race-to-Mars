#include "ObjectiveCard.h"

ObjectiveCard::ObjectiveCard(objective_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  auto card_type = static_cast<size_t>(_card_type);

  this->setCardName(card_config["OBJECTIVECARDS"][card_type]["name"]);
  // this->setCardDescription(card_config["OBJECTIVECARDS"][card_type]["description"]);
  // this->setActionPoints(card_config["OBJECTIVECARDS"][card_type]["action_points"]);
  this->setSpritePath(card_config["OBJECTIVECARDS"][card_type]["sprite_path"]);
  this->setActionPoints(card_config["OBJECTIVECARDS"][card_type]["card_id"]);

  m_card_sprite = new ScaledSprite(this->getSprtiepath());
}

void ObjectiveCard::setCardID(objective_cards _obj_card_id)
{
  this->m_cardID = _obj_card_id;
}
