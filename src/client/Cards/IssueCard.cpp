#include "IssueCard.h"

/* Setup the card */
IssueCard::IssueCard(issue_cards _card_type)
{
  card_config = file_handler.openAsJSON("CONFIGS/cards.json");
  auto card_type = static_cast<size_t>(_card_type);

  card_name = card_config["ISSUECARDS"][card_type]["name"];
  card_decription = card_config["ISSUECARDS"][card_type]["description"];
  action_points = card_config["ISSUECARDS"][card_type]["action_points"];
  cardID = static_cast<issue_cards>(card_config["ISSUECARDS"][card_type]["card_"
                                                                         "id"]);
  setSprite(card_config["ISSUECARDS"][card_type]["sprite_path"]);
  player_class_type = card_config["ISSUECARDS"][card_type]["player_class"];
}

/* Work out if the card was solved by one class */
bool IssueCard::isSolvedSolo(player_classes _player_class)
{
  switch (_player_class)
  {
    case player_classes::COMMUNICATIONS:
    {
      if (engineer_ap_assigned == 0 && medic_ap_assigned == 0 &&
          pilot_ap_assigned == 0)
      {
        return true;
      }
      break;
    }
    case player_classes::ENGINEER:
    {
      if (comms_ap_assigned == 0 && medic_ap_assigned == 0 &&
          pilot_ap_assigned == 0)
      {
        return true;
      }
      break;
    }
    case player_classes::MEDIC:
    {
      if (comms_ap_assigned == 0 && engineer_ap_assigned == 0 &&
          pilot_ap_assigned == 0)
      {
        return true;
      }
      break;
    }
    case player_classes::PILOT:
    {
      if (comms_ap_assigned == 0 && medic_ap_assigned == 0 &&
          engineer_ap_assigned == 0)
      {
        return true;
      }
      break;
    }
    default:
    {
      return false;
    }
  }
  return false;
}

/* Set the card ID */
void IssueCard::setCardID(issue_cards _item_card_id)
{
  cardID = _item_card_id;
}

/* Add action points to the card */
void IssueCard::addActionPoints(player_classes _player_class, int _ap_amount)
{
  switch (_player_class)
  {
    case player_classes::COMMUNICATIONS:
    {
      comms_ap_assigned += _ap_amount;
      break;
    }
    case player_classes::ENGINEER:
    {
      engineer_ap_assigned += _ap_amount;
      break;
    }
    case player_classes::MEDIC:
    {
      medic_ap_assigned += _ap_amount;
      break;
    }
    case player_classes ::PILOT:
    {
      pilot_ap_assigned += _ap_amount;
      break;
    }
    default:
    {
      debug_text.print("@addActionPoints - Disconnected player put action "
                       "points on card?!",
                       2);
      break;
    }
  }
  total_ap_assigned += _ap_amount;
  debug_text.print("@addActionPoints - TOTAL AP ASSIGNED TO THIS CARD: " +
                   std::to_string(total_ap_assigned));
}

/* Get assigned action points per character type */
int IssueCard::getAssignedPoints(player_classes _class)
{
  switch (_class)
  {
    case player_classes::COMMUNICATIONS:
    {
      return comms_ap_assigned;
    }
    case player_classes::ENGINEER:
    {
      return engineer_ap_assigned;
    }
    case player_classes::MEDIC:
    {
      return medic_ap_assigned;
    }
    case player_classes::PILOT:
    {
      return pilot_ap_assigned;
    }
    default:
    {
      debug_text.print("@getAssignedPoints - Tried to access points for "
                       "disconnected player.",
                       2);
      return 0;
    }
  }
}

/* Is the issue card solved */
bool IssueCard::isSolved()
{
  // return true if total assigned ap's is more than needed ap.
  if (action_points == 0)
  {
    return true;
  }
  else if (action_points != 0)
  {
    return action_points + issue_card_ap_variable <= total_ap_assigned;
  }
  return false;
}

/* Set the variable modifier to this card's AP - used for in-game modifiers */
void IssueCard::setIssueCardVariable(int _action_points)
{
  issue_card_ap_variable += _action_points;
}

/* Work out who contributed most to solving this card */
bool IssueCard::contributedMost(player_classes _player_class)
{
  switch (_player_class)
  {
    case player_classes::COMMUNICATIONS:
    {
      if (comms_ap_assigned > engineer_ap_assigned &&
          comms_ap_assigned > medic_ap_assigned &&
          comms_ap_assigned > pilot_ap_assigned)
      {
        return true;
      }
      break;
    }
    case player_classes::ENGINEER:
    {
      if (engineer_ap_assigned > comms_ap_assigned &&
          engineer_ap_assigned > medic_ap_assigned &&
          engineer_ap_assigned > pilot_ap_assigned)
      {
        return true;
      }
      break;
    }
    case player_classes::MEDIC:
    {
      if (medic_ap_assigned > engineer_ap_assigned &&
          medic_ap_assigned > comms_ap_assigned &&
          medic_ap_assigned > pilot_ap_assigned)
      {
        return true;
      }
      break;
    }
    case player_classes::PILOT:
    {
      if (pilot_ap_assigned > engineer_ap_assigned &&
          pilot_ap_assigned > comms_ap_assigned &&
          pilot_ap_assigned > medic_ap_assigned)
      {
        return true;
      }
      break;
    }
    default:
    {
      return false;
    }
  }
  return false;
}
