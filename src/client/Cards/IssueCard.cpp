#include "IssueCard.h"

void IssueCard::setCardID(issue_cards _item_card_id)
{
  m_cardID = _item_card_id;
}

void IssueCard::addActionPoints(player_classes _player_class, int _ap_amount)
{
  switch (_player_class)
  {
    case player_classes::UNASSIGNED:
    {
      debug_text.print("Unassigned player put action points on card!", 2);
      break;
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

IssueCard::IssueCard(issue_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");

  auto card_type = static_cast<size_t>(_card_type);

  m_card_name = card_config["ISSUECARDS"][card_type]["name"];
  m_card_decription = card_config["ISSUECARDS"][card_type]["description"];
  m_action_points = card_config["ISSUECARDS"][card_type]["action_points"];
  m_cardID = card_config["ISSUECARDS"][card_type]["card_id"];
  setSprite(card_config["ISSUECARDS"][card_type]["sprite_path"]);
}

bool IssueCard::isSolved()
{
  // return true if total assigned ap's is more than needed ap.
  return m_action_points + m_issue_card_ap_variable <= m_total_ap_assigned;
}

void IssueCard::setIssueCardvariable(int _action_points)
{
  m_issue_card_ap_variable += _action_points;
}
