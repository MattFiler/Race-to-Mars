#include "IssueCard.h"

void IssueCard::setCardID(issue_cards _item_card_id)
{
  this->m_cardID = _item_card_id;
  switch (static_cast<int>(_item_card_id))
  {
    case 12:
    {
    }
    case 13:
    {
    }
    case 14:
    {
    }
    case 15:
    {
    }
    case 16:
    {
    }
    case 17:
    {
    }
    default:
      break;
  }
}

void IssueCard::addActionPoints(player_classes _player_class, int _ap_amount)
{
  switch (_player_class)
  {
    case player_classes::UNASSIGNED:
    {
      throw("Unassigned player tried to assign action points. THATS "
            "IMPOSSIBRU.");
    }
    case player_classes::COMMUNICATIONS:
    {
      m_comms_ap_assigned += _ap_amount;
      break;
    }
    case player_classes::ENGINEER:
    {
      m_engineer_ap_assigned += _ap_amount;
      break;
    }
    case player_classes::MEDIC:
    {
      m_medic_ap_assigned += _ap_amount;
      break;
    }
    case player_classes ::PILOT:
    {
      m_pilot_ap_assigned += _ap_amount;
      break;
    }
    default:
      break;
  }
  m_total_ap_assigned += _ap_amount;
}

issue_cards IssueCard::getCardID()
{
  return m_cardID;
}

IssueCard::IssueCard(issue_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  auto card_type = static_cast<size_t>(_card_type);

  this->setCardName(card_config["ISSUECARDS"][card_type]["name"]);
  this->setCardDescription(card_config["ISSUECARDS"][card_type]["description"]);
  this->setActionPoints(card_config["ISSUECARDS"][card_type]["action_points"]);
  this->setSpritePath(card_config["ISSUECARDS"][card_type]["sprite_path"]);
  this->setActionPoints(card_config["ISSUECARDS"][card_type]["card_id"]);

  //  setCardName("Default");
  //  setCardDescription("Default Desc.");
  //  setActionPoints(999);
  //  setSpritePath("data/UI/CARD_IMAGES/medic_card.png");
  //  setCardID(_card_type);

  m_card_sprite = renderer->createRawSprite();
  m_card_sprite->loadTexture(getSprtiepath());
}
