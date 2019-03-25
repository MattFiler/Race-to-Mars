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
