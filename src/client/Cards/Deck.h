//
// Created by j42-king on 13/03/2019.
//

#ifndef PROJECT_DECK_H
#define PROJECT_DECK_H

#include "gamelib/FileHandler/FileHandler.h"
#include <json.hpp>
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

 private:
  std::vector<ItemCard> itm_card_deck;
  std::vector<IssueCard> iss_card_deck;
  std::vector<ObjectiveCard> obj_card_deck;
  FileHandler file_handler;
  json card_config;
};

#endif // PROJECT_DECK_H
