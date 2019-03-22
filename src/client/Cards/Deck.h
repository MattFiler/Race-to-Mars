//
// Created by j42-king on 13/03/2019.
//

#ifndef PROJECT_DECK_H
#define PROJECT_DECK_H

#include "gamelib/FileHandler/FileHandler.h"
#include "gamelib/NetworkedData/Cards.h"
#include <json.hpp>
#include <map>
#include <vector>
using json = nlohmann::json;

class ItemCard;
class IssueCard;
class ObjectiveCard;
class Card;

class Deck
{
 public:
  Deck();
  ~Deck();

  void initDecks();
  void retargetConfig(const std::string& name);

  ItemCard drawItmCard();
  IssueCard drawIssCard();
  ObjectiveCard drawObjCard();
  void shuffleDecks();
  void initLookUps();

 private:
  std::vector<ItemCard> itm_card_deck;
  std::vector<IssueCard> iss_card_deck;
  std::vector<ObjectiveCard> obj_card_deck;

  std::unordered_map<std::string, item_cards> m_itmCardLookup;
  std::unordered_map<std::string, issue_cards> m_issCardLookup;
  std::unordered_map<std::string, objective_cards> m_objCardLookup;

  FileHandler file_handler;
  json card_config;
};

#endif // PROJECT_DECK_H
