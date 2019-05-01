#ifndef PROJECT_GAMEBOARD_H
#define PROJECT_GAMEBOARD_H

#include "client/Board/ItemDeck.h"
#include "client/Board/Ship.h"
#include "client/Cards/IssueCard.h"
#include "client/Cards/ItemCard.h"
#include "client/Players/AllPlayers.h"
#include "gamelib/Constants.h"
#include "gamelib/Debug/DebugText.h"
#include <Engine/Renderer.h>

class ObjectiveCard;

/* Ideally this class will know all about stuff on the board - E.G. active
 * cards, active counters, active player tokens, etc - then all rendering can be
 * done by calling this class' render function. */

class GameBoard
{
 public:
  GameBoard() { m_players = Locator::getPlayers(); };
  ~GameBoard() = default;

  void issueTracking();
  void resetCardVariables();

  bool isHoveringOverRoom(Vector2 hover_pos);
  bool isHoveringOverIssueCard(Vector2 hover_pos);
  bool isHoveringOverItemCard(Vector2 hover_pos);
  bool isHoveringOverObjectiveCard(Vector2 hover_pos);

  ShipRoom getClickedRoom(Vector2 clicked_pos);
  IssueCard* getClickedIssueCard(Vector2 clicked_pos);
  ObjectiveCard* getClickedObjectiveCard(Vector2 clicked_pos);
  std::vector<IssueCard> getIssueCards();
  std::vector<ObjectiveCard> getObjectiveInventory()
  {
    return objective_card_inventory;
  }

  void setActiveIssueCards(int active_cards[5], bool is_new_rotation);
  void setActiveObjectiveCard(int card_index);
  void setActiveItemCard(int card_index);

  void syncIssueCards(int active_cards[5]);

  void checkissueSolved();
  bool checkObjectiveCardComplete(player_classes _this_clients_class);

  bool updateActiveIssueCards();
  bool updateActiveObjectiveCard();
  bool updateActiveItemCard(int _item_card_index);
  void eraseItemCard(int _item_card_to_delete);

  int activeIssuesCount();
  int itemCardCount();
  ObjectiveCard* getObjectiveCard();
  std::vector<ItemCard> getItemCards();
  void addObjCardToInventory();

  void clearItems() { item_inventory.clear(); }
  void useObjCardDebug();

  // issue tracking
  int engine_countdown = 2;

  bool
  assignActionPointToIssue(player_classes _class, int _issue, int _points = 1);

  ShipRoom getRoom(ship_rooms _room);

  void render(bool _obj_popup, bool _issue_popup, bool _item_popup);

  bool getPilotBlackHole() { return pilot_blackhole; }
  void setPilotBlackHole(bool _setBlackHole)
  {
    pilot_blackhole = _setBlackHole;
  }

  bool getBonusMovement() { return bonus_movement; }
  void setBonusMovement(bool _set_bonus_movement)
  {
    bonus_movement = _set_bonus_movement;
  }

  bool getGoodCommunication() { return good_communication; }
  void setGoodCommunication(bool _set_good_comm)
  {
    good_communication = _set_good_comm;
  }

  void prepReSync();

 private:
  /* Players */
  Players* m_players = nullptr;

  /* Ship */
  Ship m_ship;

  /* Cards */
  void handleIssueCardEvents(issue_cards _card_type);

  std::vector<IssueCard> active_issues;
  std::vector<ItemCard> item_inventory;
  std::vector<ObjectiveCard> objective_card_inventory;

  bool pilot_blackhole = false;
  bool bonus_movement = false;
  bool good_communication = false;

  ObjectiveCard* active_obj_card = nullptr;
  std::vector<ObjectiveCard> completed_obj_cards;
  ObjectiveCard* getObjCard() { return active_obj_card; }

  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };
  int active_item_card[5] = { -1, -1, -1, -1, -1 };
  bool item_slot_active[5] = { false, false, false, false, false };

  int objective_cards_inplay[4] = { -1, -1, -1, -1 };
  // Slot active is to keep track of available slots to place new cards since
  // if active_issue_card[x] != -1 can be overridden by another card.
  bool slot_active[5] = { false, false, false, false, false };

  bool update_issues = false;
  int new_obj_card = -1;
  CardOffsets card_offsets;

  bool lost_game = false;

  /* Misc */
  DebugText debug_text;
  GameConfig game_config;
};

#endif // PROJECT_GAMEBOARD_H
