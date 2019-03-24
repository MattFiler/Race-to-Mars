#include "IssueCard.h"

void IssueCard::setCardID(issue_cards _item_card_id)
{
  this->m_cardID = _item_card_id;
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
      m_engineer_ap_assigned += _ap_amount;
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
