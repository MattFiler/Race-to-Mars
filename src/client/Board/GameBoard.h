#ifndef PROJECT_GAMEBOARD_H
#define PROJECT_GAMEBOARD_H

#include "client/Board/ItemDeck.h"
#include "client/Board/Ship.h"
#include "client/Cards/IssueCard.h"
#include "client/Cards/ItemCard.h"
#include "client/Cards/ObjectiveCard.h"
#include "client/Players/AllPlayers.h"
#include "gamelib/Debug/DebugText.h"
#include <Engine/Renderer.h>

/* Ideally this class will know all about stuff on the board - E.G. active
 * cards, active counters, active player tokens, etc - then all rendering can be
 * done by calling this class' render function. */

enum hovered_type
{
  HOVERED_OVER_SHIP_ROOM,
  HOVERED_OVER_ISSUE_CARD,
  HOVERED_OVER_OBJECTIVE_CARD,
  DID_NOT_HOVER_OVER_ANYTHING
};

class GameBoard
{
 public:
  GameBoard() { m_players = Locator::getPlayers(); };
  ~GameBoard() = default;

  hovered_type isHoveringOverInteractable(Vector2 hover_pos);
  ShipRoom getClickedInteractableRoom(Vector2 clicked_pos);
  IssueCard* getClickedIssueCard(Vector2 clicked_pos);
  ObjectiveCard* getClickedObjectiveCard(Vector2 clicked_pos);

  void setActiveIssueCards(int active_cards[5], bool is_new_rotation);
  void setActiveObjectiveCard(int card_index);

  void updateActiveIssueCards();
  void updateActiveObjectiveCard();

  ShipRoom getRoom(ship_rooms _room);

  void render();

 private:
  /* Players */
  Players* m_players = nullptr;

  /* Ship */
  Ship m_ship;
  ShipRoom* clicked_room = nullptr;

  /* Cards */
  void handleIssueCardEvents(issue_cards _card_type);

  std::vector<IssueCard> active_issues;
  std::vector<ItemCard> item_inventory;

  bool pilot_blckhole = false;
  bool pilot_moveforward = false;

  ObjectiveCard* active_obj_card = nullptr;
  std::vector<ObjectiveCard> completed_obj_cards;

  int active_issue_cards[5] = { -1, -1, -1, -1, -1 };
  int objective_cards_inplay[4] = { -1, -1, -1, -1 };
  // Slot active is to keep track of available slots to place new cards since
  // if active_issue_card[x] != -1 can be overridden by another card.
  bool slot_active[5] = { false, false, false, false, false };

  bool update_issues = false;
  int new_obj_card = -1;

  bool lost_game = false;

  /* Misc */
  DebugText debug_text;
  GameConfig game_config;
};

#endif // PROJECT_GAMEBOARD_H
